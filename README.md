ntl &mdash; Networking Template Library
===

ntl is a C++ template library for generating efficient FPGA networking
applications with Xilinx Vivado HLS.

Directory structure
---

  * [`ntl/`](ntl/) contains the class library itself.
  * [`examples/udp-firewall/`](examples/udp-firewall/) contains an example UDP firewall application.
  * [`evaluation/hls-legacy/`](evaluation/hls-legacy/) and
    [`evaluation/p4/`](evaluation/p4) implement the same application
    without ntl, and using P4/SDNet respectively, in order to compare with the
    above example.

Dependencies
---

ntl has been tested with Xilinx Vivado HLS, and Xilinx Vivado 2018.2, and the
evaluation P4 application has been tested with Xilinx SDNet 2018.2.

The C simulation test of the UDP firewall example requires
[googletest](https://github.com/google/googletest) and
[libpcap](https://www.tcpdump.org/).
