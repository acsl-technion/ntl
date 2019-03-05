add_executable(firewall firewall.cpp main.cpp)

add_custom_target(hls
    COMMAND env GTEST_ROOT=${GTEST_ROOT} ${XILINX_VIVADO}/vivado_hls
            ${CMAKE_SOURCE_DIR}/firewall.tcl
    DEPENDS firewall.tcl csim)
