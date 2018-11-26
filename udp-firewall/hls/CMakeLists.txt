cmake_minimum_required(VERSION 3.2)

find_path(XILINX_VIVADO
    "vivado"
    PATHS /opt/Xilinx/Vivado/2018.2/bin
    DOC "Xilinx Vivado path")

set (CMAKE_CXX_STANDARD 11)
add_executable(csim firewall.cpp main.cpp)
target_include_directories(csim PRIVATE ${XILINX_VIVADO}/../include)
