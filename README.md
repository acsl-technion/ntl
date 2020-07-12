ntl — Networking Template Library
===

ntl is a C++ template library for generating efficient FPGA networking
applications with Xilinx Vivado HLS. This is a research prototype; use at your
own risk. It is free to use as a whole or in parts. Please cite our FCCM'19
paper ["Design Patterns for Code Reuse in HLS Packet Processing Pipelines"](https://haggaie.github.io/files/ntl-fccm19.pdf).

<details>
  <summary>BibTeX</summary>

    @INPROCEEDINGS{8735559,
    author={H. {Eran} and L. {Zeno} and Z. {István} and M. {Silberstein}},
    booktitle={2019 IEEE 27th Annual International Symposium on Field-Programmable
    Custom Computing Machines (FCCM)},
    title={Design Patterns for Code Reuse in {HLS} Packet Processing Pipelines},
    year={2019},
    volume={},
    number={},
    pages={208-217},
    keywords={field programmable gate arrays;high level synthesis;logic
    design;software libraries;class library;FPGA-based SmartNICs;code reuse;HLS
    packet processing pipelines;high-level synthesis;high-speed networking
    applications;UDP stateless firewall;key-value store cache;FPGA
    circuits;Optimization;Tools;C++ languages;Logic gates;Hardware;Field
    programmable gate arrays;Data structures;High level synthesis;Design
    methodology;Networking;Packet processing},
    doi={10.1109/FCCM.2019.00036},
    ISSN={2576-2613},
    month={April},}

</details>

Directory structure
---

  * [`ntl/`](ntl/) contains the class library itself.
  * [`examples/udp-firewall/`](examples/udp-firewall/) contains an example UDP firewall application.
  * [`evaluation/`](evaluation/) contains extra code and scripts for evaluating the UDP
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

ntl relies on [Boost](https://www.boost.org/) libraries, and was tested with version 1.54.0.

The C simulation test of the UDP firewall example requires
[googletest](https://github.com/google/googletest) and
[libpcap](https://www.tcpdump.org/).

Building a bitstream for the Innova card requires downloading the SDK from
Mellanox. We have used version 2768 of the SDK.
