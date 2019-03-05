package require Tcl 8.5

set common_basename [info script]
set common_basedir [file join [pwd] {*}[lrange [file split $common_basename] 0 end-1]]

proc create_project {name top dir files tb_files} {
    open_project -reset "$name-hls"

    global env common_basedir
    set_top $top
    set GTEST_ROOT $::env(GTEST_ROOT)
    set cflags "-std=gnu++0x -Wno-gnu-designator \
                -I$GTEST_ROOT/include -I$common_basedir/.."

    set ldflags "-lpcap -L$GTEST_ROOT -lgtest"

    foreach f $files {
        set f [file join $dir $f]
        add_files $f -cflags $cflags
    }

    foreach f $tb_files {
        if {![file exists $f]} {
            set f [file join $dir $f]
        }
        add_files -tb $f -cflags $cflags
    }

    open_solution "40Gbps"
    set_part {xcku060-ffva1156-2-i}
    create_clock -period "216.25MHz"
    config_rtl -prefix ${name}_
    config_interface -m_axi_addr64
    if {[llength $tb_files] > 0} {
        csim_design -ldflags $ldflags
    }
    csynth_design
    export_design -format ip_catalog
}
