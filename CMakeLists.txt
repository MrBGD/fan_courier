cmake_minimum_required(VERSION 3.26)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
# for now, the project name is used as the executable name
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")


project(${MAIN_PROJECT_NAME})
# set(CMAKE_PROJECT_VERSION_MAJOR 0)
# set(CMAKE_PROJECT_VERSION_MINOR 0)
# set(CMAKE_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################

# external dependencies with FetchContent
include(FetchContent)

set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)

# NOTE: Also update env vars used for caching in
#     - .github/actions/configure-cmake/action.yml
#     - .github/workflows/cmake.yml
FetchContent_Declare(
    PcapPlusPlus
    GIT_REPOSITORY https://github.com/seladb/PcapPlusPlus.git
    GIT_TAG        v25.05 # <which tag/branch @ <some_date>>
    GIT_SHALLOW    1  # works only with branches or tags, not with arbitrary commit hashes
    GIT_PROGRESS   ON
)

# Optimize PcapPlusPlus compilation by disabling tools/examples you don't need
set(PCAPPP_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(PCAPPP_BUILD_TUTORIALS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(PcapPlusPlus)

###############################################################################

# external dependencies with find_package

if(WIN32)
    message(STATUS "Configuring Windows Environment (Fetching WinDivert)...")
    
    # We fetch the pre-compiled binary release of WinDivert because compiling 
    # a kernel-level driver (.sys) inside a standard user CMake pipeline is heavily restricted.
    FetchContent_Declare(
        windivert_bin
        URL      https://github.com/basil00/WinDivert/releases/download/v2.2.2/WinDivert-2.2.2-A.zip
        URL_HASH SHA256=170db79f3246ebec5ef0d9f4a56a6552a4ef6e61f2215c0e1db5c105085e7ba1 # Verification hash
    )
    FetchContent_MakeAvailable(windivert_bin)
    
    # Extract the paths to the unzipped binaries
    set(WINDIVERT_INCLUDE_DIR "${windivert_bin_SOURCE_DIR}/include")
    
    # Select x64 or x86 depending on target architecture
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WINDIVERT_BIN_DIR "${windivert_bin_SOURCE_DIR}/x64")
    else()
        set(WINDIVERT_BIN_DIR "${windivert_bin_SOURCE_DIR}/x86")
    endif()
    
    # Locate the static import library (.lib) for linking
    find_library(WINDIVERT_LIB NAMES WinDivert PATHS ${WINDIVERT_BIN_DIR} NO_DEFAULT_PATH)

else()
    message(STATUS "Configuring Unix Environment (Locating libpcap)...")
    
    # Linux/macOS require the system's development headers for PCAP.
    # Note: On CI runners (like GitHub Actions), you'll want a quick step to install it:
    # Linux: apt-get install libpcap-dev  |  macOS: built-in or brew install libpcap
    find_package(Pcap REQUIRED)
endif()


# find_package(Threads REQUIRED)

###############################################################################

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing name here
add_executable(${MAIN_EXECUTABLE_NAME}
    main.cpp
)
target_sources(${MAIN_EXECUTABLE_NAME}
    PRIVATE
        src/Example.cpp
)

# NOTE: Add all defined targets (e.g. executables, libraries, etc. )
# NOTE: RUN_SANITIZERS is optional, if it's not present it will default to true
set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})
# set_compiler_flags(TARGET_NAMES ${MAIN_EXECUTABLE_NAME} ${FOO} ${BAR})
# where ${FOO} and ${BAR} represent additional executables or libraries
# you want to compile with the set compiler flags

###############################################################################

# use SYSTEM so cppcheck and clang-tidy do not report warnings from these directories
target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
    ext
)
# target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${<SomeLib>_SOURCE_DIR}/include)
# target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${<SomeLib>_BINARY_DIR}/lib)
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
    Pcap++
    Packet++
    Common++
)

# Apply platform-specific linkage
if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE ${WINDIVERT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDIVERT_LIB})
    
    # --- CRITICAL WINDOWS CI AUTOMATION STEP ---
    # Automatically copy WinDivert.dll and WinDivert64.sys to your build folder 
    # so your tests can run seamlessly without throwing runtime missing-dll errors.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${WINDIVERT_BIN_DIR}/WinDivert.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/WinDivert.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${WINDIVERT_BIN_DIR}/WinDivert64.sys"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/WinDivert64.sys"
        COMMENT "Deploying WinDivert drivers to output directory for local execution"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PCAP_LIBRARY})
endif()

###############################################################################

# copy binaries to "bin" folder; these are uploaded as artifacts on each release
# DESTINATION_DIR is set as "bin" in cmake/Options.cmake:6
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(FILES tastatura.txt config.json DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
