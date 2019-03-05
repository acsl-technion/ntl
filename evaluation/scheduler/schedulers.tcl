package require Tcl 8.5
set basename [info script]
set basedir [file join [pwd] {*}[lrange [file split $basename] 0 end-1]]
source "$basedir/../../examples/common.tcl"

create_project schedulers {sched_top} $basedir {schedulers.cpp} {}
