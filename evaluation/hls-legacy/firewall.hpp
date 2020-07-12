//
// Copyright (c) 2016-2019 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <boost/functional/hash.hpp>

enum {
    FIREWALL_ADD = 1,
    FIREWALL_DEL = 2,
};

struct hash_tag : public boost::equality_comparable<hash_tag> {
    ap_uint<32> ip_source, ip_dest;
    ap_uint<16> udp_source, udp_dest;

    hash_tag(ap_uint<32> ip_source = 0, ap_uint<32> ip_dest = 0,
             ap_uint<16> udp_source = 0, ap_uint<16> udp_dest = 0) :
        ip_source(ip_source),
        ip_dest(ip_dest),
        udp_source(udp_source),
        udp_dest(udp_dest)
    {}

    bool operator ==(const hash_tag& o) const
    {
        return ip_source == o.ip_source &&
               ip_dest == o.ip_dest &&
               udp_source == o.udp_source &&
               udp_dest == o.udp_dest;
    }
};

inline std::size_t hash_value(hash_tag tag)
{
#pragma HLS pipeline enable_flush ii=2
    std::size_t seed = 0;

    boost::hash_combine(seed, tag.ip_source.to_int());
    boost::hash_combine(seed, tag.ip_dest.to_int());
    boost::hash_combine(seed, tag.udp_source.to_short());
    boost::hash_combine(seed, tag.udp_dest.to_short());

    return seed;
}

struct gateway_data
{
        hash_tag tag;
        ap_uint<1> result;
        int status;
};

struct gateway_registers {
    gateway_registers() : cmd(), data(), done(0) {}

    struct {
        ap_uint<31> addr;
        ap_uint<1>  go; // Bit 31
    } cmd;

    gateway_data data;

    ap_uint<1> done;
};

typedef hls::stream<ap_uint<1> > bool_stream;

void firewall_top(hls::stream<ntl_legacy::raw_axi_data>& in,
                  hls::stream<ntl_legacy::raw_axi_data>& data_out,
                  bool_stream& classify_out, gateway_registers& g);
