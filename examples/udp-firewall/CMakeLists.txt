add_executable(firewall firewall.cpp main.cpp)

add_hls_target(firewall-hls firewall.tcl firewall)
