cmake_minimum_required(VERSION 3.2)

find_path(XILINX_VIVADO
    "vivado"
    PATHS /opt/Xilinx/Vivado/2018.2/bin
    DOC "Xilinx Vivado path")

set(GTEST_ROOT "$ENV{GTEST_ROOT}" CACHE PATH "Root directory of gtest installation")
find_package(GTest REQUIRED)

set (CMAKE_CXX_STANDARD 11)
add_executable(csim firewall.cpp main.cpp)
target_include_directories(csim PRIVATE ${XILINX_VIVADO}/../include ${GTEST_INCLUDE_DIRS} ../..)
target_link_libraries(csim ${GTEST_LIBRARIES} pcap Threads::Threads)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
