add_custom_command(OUTPUT firewall.sdnet
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/firewall.p4
    COMMAND ${XILINX_SDNET}/p4c-sdnet -o firewall.sdnet ${CMAKE_CURRENT_SOURCE_DIR}/firewall.p4)
add_custom_command(OUTPUT firewall/XilinxSwitch/XilinxSwitch.v
    firewall/XilinxSwitch/XilinxSwitch.TB/XilinxSwitch.cpp
    firewall/XilinxSwitch/XilinxSwitch.TB/sdnet_lib.cpp
    DEPENDS firewall.sdnet
    COMMAND ${XILINX_SDNET}/sdnet -workDir firewall -busWidth 256 -controlClock 216.25 -lineClock 216.25 -lookupClock 216.25 firewall.sdnet)

add_custom_target(firewall-p4
    DEPENDS firewall/XilinxSwitch/XilinxSwitch.v
    WORKING_DIRECTORY firewall/XilinxSwitch
    COMMAND sed -e 's/xcvu095-ffva2104-2-e/xcku060-ffva1156-2-i/' 
                -e 's/xcvu9p-flgc2104-2-e/xcku060-ffva1156-2-i/' --in-place XilinxSwitch_vivado_packager.tcl && 
            rm -rf XilinxSwitch_vivado && 
            ${XILINX_VIVADO}/vivado -mode batch -source XilinxSwitch_vivado_packager.tcl)

