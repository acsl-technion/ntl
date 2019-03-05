ntl &mdash; Networking Template Library
===

ntl is a C++ template library for generating efficient FPGA networking
applications with Xilinx Vivado HLS.

Directory structure
---

  * [`ntl/`](ntl/) contains the class library itself.
  * [`examples/udp-firewall/`](examples/udp-firewall/) contains an example UDP firewall application.
  * [`evaluation/`] contains extra code and scripts for evaluating the UDP
    firewall example on a Mellanox Innova Flex SmartNIC and comparing against
    alternative implementations:

    * [`hls-legacy/`](evaluation/hls-legacy/) and
      [`p4/`](evaluation/p4/) implement the same application
      without ntl, and using P4/SDNet respectively, in order to compare with the
      ntl example.
    * [`scripts/`](evaluation/scripts/), [`verilog/`](evaluation/verilog/),
      and [`xci/`](evaluation/xci/) provide necessary code to build a bitstream for the
      Innova.

Dependencies
---

ntl has been tested with Xilinx Vivado HLS, and Xilinx Vivado 2018.2, and the
evaluation P4 application has been tested with Xilinx SDNet 2018.2.

The C simulation test of the UDP firewall example requires
[googletest](https://github.com/google/googletest) and
[libpcap](https://www.tcpdump.org/).
