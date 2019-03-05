add_executable(schedulers schedulers.cpp)

add_hls_target(schedulers-hls schedulers.tcl schedulers)
