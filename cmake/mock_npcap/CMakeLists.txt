# cmake/mock_npcap/CMakeLists.txt
# Builds mock stub libraries for CI builds when the real npcap SDK is not available.
# This file is included by the main CMakeLists.txt on Windows when BUILDING_FOR_CI is set.

cmake_minimum_required(VERSION 3.26)

# Mock libpcap library
add_library(mock_libpcap STATIC
    src/mock_pcap.c
)
target_include_directories(mock_libpcap PUBLIC
    include
)
target_compile_definitions(mock_libpcap PRIVATE
    PCAP_ERRBUF_SIZE=256
    DLT_EN10MB=1
)

# Mock Packet.dll library
add_library(mock_packet STATIC
    src/mock_packet.c
)
target_include_directories(mock_packet PUBLIC
    include
)

# Mock light_pcapng library
add_library(mock_light_pcapng STATIC
    src/mock_light_pcapng.c
)
target_include_directories(mock_light_pcapng PUBLIC
    include
)

# Export targets for use by other CMake files
set(MOCK_NPCAP_LIBRARIES
    mock_libpcap
    mock_packet
    mock_light_pcapng
)
