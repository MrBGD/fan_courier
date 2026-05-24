cmake_minimum_required(VERSION 3.26)

add_library(mock_libpcap SHARED src/mock_pcap.c)
target_include_directories(mock_libpcap PUBLIC ${NPCAP_REAL_INCLUDE_DIR})
target_compile_definitions(mock_libpcap PRIVATE PCAP_ERRBUF_SIZE=256 DLT_EN10MB=1 WPCAP HAVE_REMOTE BUILDING_PCAP)
set_target_properties(mock_libpcap PROPERTIES
        OUTPUT_NAME "wpcap"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_library(mock_packet SHARED src/mock_packet.c)
target_include_directories(mock_packet PUBLIC ${NPCAP_REAL_INCLUDE_DIR})
target_compile_definitions(mock_packet PRIVATE WPCAP HAVE_REMOTE BUILDING_PCAP)
set_target_properties(mock_packet PROPERTIES
        OUTPUT_NAME "Packet"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_library(mock_light_pcapng SHARED src/mock_light_pcapng.c)
target_include_directories(mock_light_pcapng PUBLIC ${NPCAP_REAL_INCLUDE_DIR})
set_target_properties(mock_light_pcapng PROPERTIES
        OUTPUT_NAME "light_pcapng"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)