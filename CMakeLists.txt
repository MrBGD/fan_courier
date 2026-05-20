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
if(UNIX)
    #if(APPLE)
    #    find_package(PcapPlusPlus)
    #else()
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
    #endif()
else()
    if(MINGW)
        set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-mingw64-x86_64-gcc-15.1.0.zip")
        set(PCPP_SUBDIR "pcapplusplus-25.05-windows-mingw64-x86_64-gcc-15.1.0")
    elseif(MSVC)
        # Check if we are building in Debug or Release mode to choose the matching MSVC asset
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-vs2022-x64-debug.zip")
            set(PCPP_SUBDIR "pcapplusplus-25.05-windows-vs2022-x64-debug")
        else()
            set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-vs2022-x64-release.zip")
            set(PCPP_SUBDIR "pcapplusplus-25.05-windows-vs2022-x64-release")
        endif()
    else()
        message(FATAL_ERROR "Unsupported toolchain. This configuration only supports MinGW or MSVC.")
    endif()

    # 2. Declare and Populate the content using FetchContent
    FetchContent_Declare(
        pcapplusplus
        URL ${PCPP_URL}
        DOWNLOAD_ONLY TRUE
    )

    FetchContent_MakeAvailable(pcapplusplus)

    # 3. Create interface/imported targets to map the downloaded binaries
    # Note: PcapPlusPlus relies on Common++, Packet++, and Pcap++
    set(PCPP_ROOT "${pcapplusplus_SOURCE_DIR}/${PCPP_SUBDIR}")
    set(PCPP_INCLUDE_DIR "${PCPP_ROOT}/include/pcapplusplus")
    set(PCPP_LIB_DIR "${PCPP_ROOT}/lib")

    # Helper function to define imported static libraries
    function(add_pcpp_library TARGET_NAME LIB_FILE_NAME)
        add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
        set_target_properties(${TARGET_NAME} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PCPP_INCLUDE_DIR}"
            IMPORTED_LOCATION "${PCPP_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${LIB_FILE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )
    endfunction()

    # Define the three PcapPlusPlus libraries
    add_pcpp_library(PcapPlusPlus::Common++ "Common++")
    add_pcpp_library(PcapPlusPlus::Packet++ "Packet++")
    add_pcpp_library(PcapPlusPlus::Pcap++ "Pcap++")

    # Combine them into a single target for ease of use
    add_library(PcapPlusPlus::PcapPlusPlus INTERFACE IMPORTED GLOBAL)

endif()

###############################################################################

# external dependencies with find_package

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

if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
         PCAP::PCAP Packet::Packet PcapPlusPlus::Pcap++ PcapPlusPlus::Common++
    )
else()
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
        Pcap++ Common++
    )
endif()

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
