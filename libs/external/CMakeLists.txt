set(CMAKE_BUILD_TYPE Release)

add_subdirectory(libpcap)

set(NPCAP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/external/libpcap/lib/include")
set(NPCAP_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/external/libpcap/lib/lib")