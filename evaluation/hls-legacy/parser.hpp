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

#include "ntl-legacy/dup.hpp"
#include "ntl-legacy/link.hpp"
#include "ntl-legacy/axi_data.hpp"

#include <net/ethernet.h>
#include <netinet/in.h>

typedef hls::stream<ntl_legacy::axi_data> axi_data_stream;

struct metadata {
    ap_uint<32> ip_source, ip_dest;
    ap_uint<16> ether_type, udp_source, udp_dest;
    ap_uint<8> ip_protocol;

    metadata() :
        ip_source(0), ip_dest(0), ether_type(0), udp_source(0), udp_dest(0),
        ip_protocol(0)
    {}

};

inline bool valid_ip(metadata m)
{
#pragma HLS inline
    return m.ether_type == ETHERTYPE_IP;
}

inline bool valid_udp(metadata m)
{
#pragma HLS inline
    return valid_ip(m) && m.ip_protocol == IPPROTO_UDP;
}

typedef hls::stream<metadata> metadata_stream;

template <unsigned start, unsigned end, typename T>
ap_uint<8 * (end - start)> range(const T& val)
{
    // static_assert(end > start, "Invalid range.");

    const int width = T::width;

    return val(width - 8 * start - 1, width - 8 * end);
}

inline void parser(axi_data_stream& in, metadata_stream& out)
{
#pragma HLS pipeline
    static enum { IDLE, FIRST, REST } state = IDLE;
    static metadata ret;

    ntl_legacy::axi_data flit;

    switch (state) {
    case IDLE:
        if (in.empty() || out.full())
            return;

        in.read_nb(flit);
        ret = metadata();
        ret.ether_type = range<12, 14>(flit.data);
        ret.ip_protocol = range<23, 24>(flit.data);
        ret.ip_source = range<26, 30>(flit.data);
        ret.ip_dest(31, 16) = range<30, 32>(flit.data);
        state = flit.last ? IDLE : FIRST;
        if (flit.last)
            out.write_nb(ret);
        break;
    case FIRST:
        if (in.empty() || out.full())
            return;

        in.read_nb(flit);
        ret.ip_dest(15, 0) = range<0, 2>(flit.data);
        ret.udp_source = range<2, 4>(flit.data);
        ret.udp_dest = range<4, 6>(flit.data);
        out.write_nb(ret);
        state = flit.last ? IDLE : REST;
        break;
    case REST:
        if (in.empty())
            return;

        in.read_nb(flit);
        state = flit.last ? IDLE : REST;
        break;
    }
}
