cmake_minimum_required(VERSION 3.2)

find_path(XILINX_VIVADO
    "vivado"
    PATHS /opt/Xilinx/Vivado/2018.2/bin
    DOC "Xilinx Vivado path")

set(GTEST_ROOT "$ENV{GTEST_ROOT}" CACHE PATH "Root directory of gtest installation")
find_package(GTest REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (CMAKE_CXX_STANDARD 11)

include_directories(${XILINX_VIVADO}/../include ${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
link_libraries(${GTEST_LIBRARIES} pcap Threads::Threads)

option(BUILD_EVALUATION "Build the evaluation subdirectory contents" OFF)

set(VIVADO_HLS "env GTEST_ROOT=${GTEST_ROOT} ${XILINX_VIVADO}/vivado_hls")
function(add_hls_target target_name tcl deps)
    add_custom_target(${target_name}
        COMMAND env GTEST_ROOT=${GTEST_ROOT} ${XILINX_VIVADO}/vivado_hls ${CMAKE_CURRENT_SOURCE_DIR}/${tcl}
        DEPENDS ${tcl} ${CMAKE_SOURCE_DIR}/examples/common.tcl ${deps})
endfunction()

add_custom_target(hls)

add_subdirectory(examples/udp-firewall/)
add_dependencies(hls firewall-hls)
if(BUILD_EVALUATION)
    add_subdirectory(evaluation/scheduler/)
    add_dependencies(hls schedulers-hls)
    add_subdirectory(evaluation/hls-legacy/)
    add_dependencies(hls firewall-legacy-hls)
endif()

