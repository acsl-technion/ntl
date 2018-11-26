package require Tcl 8.5

proc create_project {name top dir files tb_files} {
    open_project -reset "$name"

    global env
    set_top $top
    set cflags "-std=gnu++0x \
                -Wno-gnu-designator"

    set ldflags "-lpcap"

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

create_project firewall {firewall_top} . {firewall.cpp} {main.cpp}
