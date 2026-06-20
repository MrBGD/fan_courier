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
include(FetchContent)
set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)

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

    target_compile_options(Common++   INTERFACE -Wno-pedantic -Wno-c++98-compat)
    target_compile_options(Packet++   INTERFACE -Wno-pedantic -Wno-c++98-compat)
    target_compile_options(Pcap++     INTERFACE -Wno-pedantic -Wno-c++98-compat)
    target_compile_options(memplumber INTERFACE -Wno-pedantic -Wno-c++98-compat)
else()
    set(CMAKE_TLS_VERIFY OFF)
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

    file(GLOB PCPP_SUBDIRS "${pcapplusplus_SOURCE_DIR}/pcapplusplus-*")
    if(PCPP_SUBDIRS)
        list(GET PCPP_SUBDIRS 0 PCPP_SUBDIR)
    else()
        set(PCPP_SUBDIR "${pcapplusplus_SOURCE_DIR}")
    endif()

    if(MSVC)
        set(PCPP_LIB_EXT "lib")
        set(PCPP_LIB_PREFIX "")
    else()
        set(PCPP_LIB_EXT "a")
        set(PCPP_LIB_PREFIX "lib")
    endif()

    find_package(Threads REQUIRED)

    add_library(PcapPlusPlus::Common++ STATIC IMPORTED)
    set_target_properties(PcapPlusPlus::Common++ PROPERTIES
            IMPORTED_LOCATION "${PCPP_SUBDIR}/lib/${PCPP_LIB_PREFIX}Common++.${PCPP_LIB_EXT}"
            INTERFACE_INCLUDE_DIRECTORIES "${PCPP_SUBDIR}/include/pcapplusplus;${PCPP_SUBDIR}/include"
            INTERFACE_LINK_LIBRARIES "ws2_32;iphlpapi"
    )
    add_library(PcapPlusPlus::Packet++ STATIC IMPORTED)
    set_target_properties(PcapPlusPlus::Packet++ PROPERTIES
            IMPORTED_LOCATION "${PCPP_SUBDIR}/lib/${PCPP_LIB_PREFIX}Packet++.${PCPP_LIB_EXT}"
            INTERFACE_INCLUDE_DIRECTORIES "${PCPP_SUBDIR}/include/pcapplusplus;${PCPP_SUBDIR}/include"
            INTERFACE_LINK_LIBRARIES "PcapPlusPlus::Common++"
    )
    add_library(PcapPlusPlus::Pcap++ STATIC IMPORTED)
    set_target_properties(PcapPlusPlus::Pcap++ PROPERTIES
            IMPORTED_LOCATION "${PCPP_SUBDIR}/lib/${PCPP_LIB_PREFIX}Pcap++.${PCPP_LIB_EXT}"
            INTERFACE_INCLUDE_DIRECTORIES "${PCPP_SUBDIR}/include/pcapplusplus;${PCPP_SUBDIR}/include"
            INTERFACE_LINK_LIBRARIES "ws2_32;iphlpapi;Packet::Packet;PcapPlusPlus::Common++;PcapPlusPlus::Packet++;PCAP::PCAP;Threads::Threads"
    )

    if(NOT TARGET PCAP::PCAP)
        if(DEFINED ENV{BUILDING_FOR_CI} OR NPCAP_SDK_PATH STREQUAL "")
            add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/mock_npcap)
            add_library(PCAP::PCAP IMPORTED INTERFACE)
            set_target_properties(PCAP::PCAP PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/cmake/mock_npcap/include"
                    INTERFACE_LINK_LIBRARIES "mock_libpcap"
            )
        else()
            set(NPCAP_SDK "$ENV{NPCAP_SDK_PATH}")
            find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h PATHS "${NPCAP_SDK}/Include" NO_DEFAULT_PATH)
            find_library(PCAP_LIBRARY NAMES wpcap pcap PATHS "${NPCAP_SDK}/Lib/x64" NO_DEFAULT_PATH)
            if(PCAP_INCLUDE_DIR AND PCAP_LIBRARY)
                add_library(PCAP::PCAP IMPORTED SHARED)
                set_target_properties(PCAP::PCAP PROPERTIES
                        IMPORTED_LOCATION "${PCAP_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${PCAP_INCLUDE_DIR}"
                )
            else()
                message(FATAL_ERROR "npcap SDK not found. Set NPCAP_SDK_PATH or BUILDING_FOR_CI=1.")
            endif()
        endif()
    endif()

    if(NOT TARGET Packet::Packet)
        if(DEFINED ENV{BUILDING_FOR_CI} OR NPCAP_SDK_PATH STREQUAL "")
            add_library(Packet::Packet IMPORTED INTERFACE)
            set_target_properties(Packet::Packet PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/cmake/mock_npcap/include"
                    INTERFACE_LINK_LIBRARIES "mock_packet"
            )
        else()
            set(NPCAP_SDK "$ENV{NPCAP_SDK_PATH}")
            find_path(PACKET_INCLUDE_DIR NAMES Packet32.h PATHS "${NPCAP_SDK}/Include" NO_DEFAULT_PATH)
            find_library(PACKET_LIBRARY NAMES Packet PATHS "${NPCAP_SDK}/Lib/x64" NO_DEFAULT_PATH)
            if(PACKET_INCLUDE_DIR AND PACKET_LIBRARY)
                add_library(Packet::Packet IMPORTED SHARED)
                set_target_properties(Packet::Packet PROPERTIES
                        IMPORTED_LOCATION "${PACKET_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${PACKET_INCLUDE_DIR}"
                )
            else()
                message(FATAL_ERROR "npcap SDK Packet lib not found. Set NPCAP_SDK_PATH or BUILDING_FOR_CI=1.")
            endif()
        endif()
    endif()

    if(NOT TARGET mock_light_pcapng AND (DEFINED ENV{BUILDING_FOR_CI} OR NPCAP_SDK_PATH STREQUAL ""))
        add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/mock_npcap)
    endif()
endif()

###############################################################################

add_executable(${MAIN_EXECUTABLE_NAME} main.cpp)
target_sources(${MAIN_EXECUTABLE_NAME}
        PRIVATE
        src/cmd_exec.cpp
        src/dashboard_panel.cpp
        src/guest_user.cpp
        src/login_panel.cpp
        src/network.cpp
        src/network_packet.cpp
        src/packet_observer.cpp
        src/panel.cpp
        src/report_panel.cpp
        src/user.cpp
)

set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ext)

if(WIN32)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE PcapPlusPlus::Pcap++)
else()
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE Pcap++ Common++)
endif()

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PCAP_LIBRARY})
endif()

###############################################################################

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt users.json DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})