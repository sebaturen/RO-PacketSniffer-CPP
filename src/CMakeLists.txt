# base libs~~~~~~~~~~~~~
find_package(CURL CONFIG REQUIRED)
message("finded!")
# npcap lib~
set(NPCAP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/external/libpcap/include")
set(NPCAP_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/external/libpcap/lib/x64")

set(EXTERNAL_LIBS
    "${NPCAP_LIB_DIR}/wpcap.lib"
    "${NPCAP_LIB_DIR}/packet.lib"
)
set(EXTERNAL_INCLUDES
	"${NPCAP_INCLUDE_DIR}"
)
include_directories(${EXTERNAL_INCLUDES})

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.h)

add_executable(ROSniffer main.cpp)
set_target_properties(ROSniffer PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Include SRC content file
target_sources(ROSniffer PRIVATE ${SRC_FILES})
target_sources(ROSniffer PRIVATE ${HEADER_FILES})

# Include libs
target_include_directories(ROSniffer PRIVATE ${EXTERNAL_INCLUDES})
target_link_libraries(ROSniffer PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(ROSniffer PRIVATE CURL::libcurl)
target_link_libraries(ROSniffer PRIVATE ${EXTERNAL_LIBS})
