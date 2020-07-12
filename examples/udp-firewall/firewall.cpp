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

#include "ntl/map.hpp"
#include "parser.hpp"
#include "firewall.hpp"

#include <functional>

#include <boost/operators.hpp>

void firewall::step(axi_data_stream& in, axi_data_stream& data_out, bool_stream& classify_out, gateway_registers& g)
{
#pragma HLS inline
#pragma HLS stream variable=invalid_udp depth=16
    gateway.gateway(g, [&](int addr, gateway_data& data) -> int { return rpc(addr, data); });

    dup_data.step(in);
    parse.step(dup_data._streams[0]);
    dup_metadata.step(parse.out);

    ntl::map(dup_metadata._streams[0], hash.lookups, [](const metadata& m) {
        return hash_tag{m.ip_source, m.ip_dest, m.udp_source, m.udp_dest};
    });

    hash.hash_table();
    
    ntl::map(dup_metadata._streams[1], invalid_udp, [](const metadata& m) {
        return !m.valid_udp();
    });

    ntl::map(hash.results, result_with_default, [](const ntl::maybe<std::tuple<uint32_t, hash_t::mapped_type> >& val) {
        return val.valid() ? std::get<1>(val.value()) : ap_uint<1>(0);
    });

    merge_hash_results.step(std::logical_or<ap_uint<1> >(),
                            result_with_default, invalid_udp);

    link(merge_hash_results.out, classify_out);
    link(dup_data._streams[1], data_out);
}

int firewall::rpc(int addr, gateway_data& data)
{
    switch (addr) {
    case FIREWALL_ADD:
        return hash.gateway_add_entry(std::make_tuple(data.tag, data.result), &data.status);
    case FIREWALL_DEL:
        return hash.gateway_delete_entry(data.tag, &data.status);
    default:
        return ntl::GW_FAIL;
    }
}

void firewall_top(ntl::stream<ntl::raw_axi_data, ntl::axi_stream_tag>& in,
                  ntl::stream<ntl::raw_axi_data, ntl::axi_stream_tag>& data_out,
                  bool_stream& classify_out, gateway_registers& g)
{
#pragma HLS dataflow
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=in._stream
#pragma HLS interface axis port=data_out._stream

    GATEWAY_OFFSET(g, 0x100, 0x118, 0xfc)
    static firewall f;
    static axi_data_stream in_fifo, out_fifo;
    ntl::link(in, in_fifo);
#pragma HLS stream variable=out_fifo depth=16
    ntl::link(out_fifo, data_out);
    f.step(in_fifo, out_fifo, classify_out, g);
}
