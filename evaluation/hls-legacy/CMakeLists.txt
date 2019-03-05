set (CMAKE_CXX_STANDARD 98)
add_executable(firewall-legacy firewall.cpp main.cpp)

add_hls_target(firewall-legacy-hls firewall.tcl firewall-legacy)
