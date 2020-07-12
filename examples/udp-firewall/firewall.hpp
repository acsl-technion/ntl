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

#include "ntl/cache.hpp"

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

    hash_tag(const hash_tag& o) = default;

    bool operator ==(const hash_tag& o) const
    {
        return ip_source == o.ip_source &&
               ip_dest == o.ip_dest &&
               udp_source == o.udp_source &&
               udp_dest == o.udp_dest;
    }
};

namespace ntl {
    template <>
    struct pack<hash_tag> {
        static const int width = 96;

        static ap_uint<width> to_int(const hash_tag& e) {
            return (e.ip_source, e.ip_dest, e.udp_source, e.udp_dest);
        }

        static hash_tag from_int(const ap_uint<width>& d) {
            return hash_tag{
                range<0, 4>(d),
                range<4, 8>(d),
                range<8, 10>(d),
                range<10, 12>(d)};
        }
    };
}

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

typedef ntl::gateway_registers<gateway_data> gateway_registers;

typedef ntl::stream<ap_uint<1> > bool_stream;
typedef ntl::hash_table_wrapper<hash_tag, ap_uint<1>, 1024> hash_t;

class firewall
{
public:
    void step(axi_data_stream& in, axi_data_stream& data_out, bool_stream& classify_out, gateway_registers& g);
    int rpc(int addr, gateway_data& data);

private:
    ntl::gateway_impl<gateway_data> gateway;

    ntl::dup<ntl::axi_data, 2> dup_data;
    ntl::dup<metadata, 2> dup_metadata;
    parser parse;
    hash_t hash;
    bool_stream invalid_udp;
    bool_stream result_with_default;
    ntl::zip_with<ap_uint<1>, ap_uint<1>, ap_uint<1> > merge_hash_results;
};
