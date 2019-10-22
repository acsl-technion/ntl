cmake_minimum_required(VERSION 3.2)

find_path(XILINX_VIVADO
    "vivado"
    PATHS /opt/Xilinx/Vivado/2018.2/bin
    DOC "Xilinx Vivado path")

find_path(XILINX_SDNET
    "sdnet"
    PATHS /opt/Xilinx/SDNet/2018.2/bin
    DOC "Xilinx SDNet path")

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
add_custom_target(p4)

add_subdirectory(examples/udp-firewall/)
add_dependencies(hls firewall-hls)
if(BUILD_EVALUATION)
    add_subdirectory(evaluation/scheduler/)
    add_dependencies(hls schedulers-hls)
    add_subdirectory(evaluation/hls-legacy/)
    add_dependencies(hls firewall-legacy-hls)
    add_subdirectory(evaluation/p4)
    add_dependencies(p4 firewall-p4)
endif()

enable_testing()
# add a check target to build all tests and run them
# see https://cmake.org/Wiki/CMakeEmulateMakeCheck
add_custom_target(check COMMAND env PATH=${TCPDUMP_PATH}:$$PATH ${CMAKE_CTEST_COMMAND})

function(add_gtest name)
    cmake_parse_arguments(GTEST
        ""
        ""
        "SOURCES"
        ${ARGN}
    )

    add_executable(${name} EXCLUDE_FROM_ALL ${GTEST_SOURCES})
    add_dependencies(check ${name})
    add_test(${name} ${name})
    #target_compile_features(${name} PRIVATE cxx_lambdas)
    target_link_libraries(${name} ${GTEST_LIBRARIES}) #  ${UUID_LIBRARIES} Threads::Threads)
endfunction(add_gtest)

add_subdirectory(tests)
