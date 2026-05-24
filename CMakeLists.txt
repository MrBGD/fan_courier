cmake_minimum_required(VERSION 3.26)

set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################
# EXTERNAL DEPENDENCIES & FETCH
###############################################################################
include(FetchContent)
set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)

# Threading este necesar intern de PcapPlusPlus
find_package(Threads REQUIRED)

if(UNIX)
    FetchContent_Declare(
            PcapPlusPlus
            GIT_REPOSITORY https://github.com/seladb/PcapPlusPlus.git
            GIT_TAG        v25.05
            GIT_SHALLOW    1
            GIT_PROGRESS   ON
    )
    set(PCAPPP_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(PCAPPP_BUILD_TUTORIALS OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(PcapPlusPlus)
else()
    # ==============================================================================
    # 1. DYNAMICALLY FETCH OFFICIAL NPCAP SDK
    # ==============================================================================
    message(STATUS "Fetching official Npcap SDK...")
    FetchContent_Declare(
            npcap_sdk
            URL "https://npcap.com/dist/npcap-sdk-1.13.zip"
    )
    FetchContent_MakeAvailable(npcap_sdk)
    set(NPCAP_SDK_DIR "${npcap_sdk_SOURCE_DIR}")
    set(NPCAP_REAL_INCLUDE_DIR "${NPCAP_SDK_DIR}/Include")

    # ==============================================================================
    # 2. PROVIDE PCAP::PCAP and Packet::Packet
    # ==============================================================================
    if(DEFINED ENV{BUILDING_FOR_CI})
        message(STATUS "### Windows: Configuring MOCK Npcap Build ###")

        add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/mock_npcap)

        if(NOT TARGET PCAP::PCAP)
            add_library(PCAP::PCAP IMPORTED INTERFACE)
            set_target_properties(PCAP::PCAP PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${NPCAP_REAL_INCLUDE_DIR}"
                    INTERFACE_LINK_LIBRARIES "mock_libpcap"
            )
        endif()

        if(NOT TARGET Packet::Packet)
            add_library(Packet::Packet IMPORTED INTERFACE)
            set_target_properties(Packet::Packet PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${NPCAP_REAL_INCLUDE_DIR}"
                    INTERFACE_LINK_LIBRARIES "mock_packet"
            )
        endif()
    else()
        message(STATUS "### Windows: Configuring REAL Npcap Build ###")

        if(NOT TARGET PCAP::PCAP)
            find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h PATHS "${NPCAP_SDK_DIR}/Include" NO_DEFAULT_PATH)
            find_library(PCAP_LIBRARY NAMES wpcap pcap PATHS "${NPCAP_SDK_DIR}/Lib/x64" NO_DEFAULT_PATH)
            if(PCAP_INCLUDE_DIR AND PCAP_LIBRARY)
                add_library(PCAP::PCAP IMPORTED SHARED)
                set_target_properties(PCAP::PCAP PROPERTIES
                        IMPORTED_LOCATION "${PCAP_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${PCAP_INCLUDE_DIR}"
                )
            else()
                message(FATAL_ERROR "Real npcap SDK could not be configured.")
            endif()
        endif()

        if(NOT TARGET Packet::Packet)
            find_path(PACKET_INCLUDE_DIR NAMES Packet32.h PATHS "${NPCAP_SDK_DIR}/Include" NO_DEFAULT_PATH)
            find_library(PACKET_LIBRARY NAMES Packet PATHS "${NPCAP_SDK_DIR}/Lib/x64" NO_DEFAULT_PATH)
            if(PACKET_INCLUDE_DIR AND PACKET_LIBRARY)
                add_library(Packet::Packet IMPORTED SHARED)
                set_target_properties(Packet::Packet PROPERTIES
                        IMPORTED_LOCATION "${PACKET_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${PACKET_INCLUDE_DIR}"
                )
            else()
                message(FATAL_ERROR "Real npcap SDK Packet library not found.")
            endif()
        endif()
    endif()

    # ==============================================================================
    # 3. TRICK CMAKE SO PCAPPLUSPLUS DOESN'T SILENTLY FAIL
    # Creăm două module false pentru a satisface 'find_dependency' din PcapPlusPlus
    # ==============================================================================
    file(WRITE "${CMAKE_BINARY_DIR}/dummy_modules/FindPCAP.cmake" "set(PCAP_FOUND TRUE)")
    file(WRITE "${CMAKE_BINARY_DIR}/dummy_modules/FindPacket.cmake" "set(Packet_FOUND TRUE)")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/dummy_modules")

    # ==============================================================================
    # 4. DOWNLOAD & CONFIGURE PCAPPLUSPLUS
    # ==============================================================================
    if(MINGW)
        set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-mingw64-x86_64-gcc-15.1.0.zip")
    elseif(MSVC)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-vs2022-x64-debug.zip")
        else()
            set(PCPP_URL "https://github.com/seladb/PcapPlusPlus/releases/download/v25.05/pcapplusplus-25.05-windows-vs2022-x64-release.zip")
        endif()
    endif()

    FetchContent_Declare(
            pcapplusplus
            DOWNLOAD_ONLY TRUE
            URL "${PCPP_URL}"
    )
    FetchContent_MakeAvailable(pcapplusplus)

    set(PCPP_ROOT "${pcapplusplus_SOURCE_DIR}")
    file(GLOB PCPP_SUBDIRS "${pcapplusplus_SOURCE_DIR}/pcapplusplus-*")
    if(PCPP_SUBDIRS)
        list(GET PCPP_SUBDIRS 0 PCPP_SUBDIR)
    else()
        set(PCPP_SUBDIR "${pcapplusplus_SOURCE_DIR}")
    endif()
    set(PCPP_CMAKE_DIR "${PCPP_SUBDIR}/lib/cmake/pcapplusplus")

    # Forțăm PcapPlusPlus să se încarce robust cu find_package
    set(PcapPlusPlus_DIR "${PCPP_CMAKE_DIR}")
    find_package(PcapPlusPlus REQUIRED CONFIG PATHS "${PCPP_CMAKE_DIR}" NO_DEFAULT_PATH)

endif()

###############################################################################
# EXECUTABLES & SOURCES
###############################################################################
add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
)
target_sources(${MAIN_EXECUTABLE_NAME}
        PRIVATE
        src/dashboard_panel.cpp
        src/guest_user.cpp
        src/login_panel.cpp
        src/network.cpp
        src/network_packet.cpp
        src/packet_observer.cpp
        src/panel.cpp
        src/user.cpp
)

set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
        ext
)

find_package(PCAP REQUIRED)

# Forcefully set the IMPLIB properties for Windows MSVC builds
if(WIN32)
    # Patch PCAP
    if(TARGET PCAP::PCAP)
        set_target_properties(PCAP::PCAP PROPERTIES
                IMPORTED_IMPLIB_DEBUG "${PCAP_LIBRARY}"
                IMPORTED_IMPLIB_RELEASE "${PCAP_LIBRARY}"
                IMPORTED_IMPLIB "${PCAP_LIBRARY}"
        )
    endif()

    # Patch Packet
    if(TARGET Packet::Packet)
        set_target_properties(Packet::Packet PROPERTIES
                IMPORTED_IMPLIB_DEBUG "${PACKET_LIBRARY}"
                IMPORTED_IMPLIB_RELEASE "${PACKET_LIBRARY}"
                IMPORTED_IMPLIB "${PACKET_LIBRARY}"
        )
    endif()
endif()


if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
            PcapPlusPlus::Pcap++
            Threads::Threads
    )
else()
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
            Pcap++ Common++ Threads::Threads
    )
endif()

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt users.json DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})