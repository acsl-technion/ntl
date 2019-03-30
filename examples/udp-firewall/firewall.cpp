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

void firewall_top(ntl::stream<ntl::raw_axi_data, ntl::axi_stream_input_tag>& in,
                  ntl::stream<ntl::raw_axi_data, ntl::axi_stream_output_tag>& data_out,
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
