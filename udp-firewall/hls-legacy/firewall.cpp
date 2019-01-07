#include "parser.hpp"
#include "firewall.hpp"
#include "cache.hpp"

int firewall_rpc(int addr, gateway_data& data,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)
{
#pragma HLS inline
    switch (addr) {
    case FIREWALL_ADD:
        return gateway_add_entry(std::make_pair(data.tag, data.result),
                                 &data.status, gateway_commands,
                                 gateway_responses);
    case FIREWALL_DEL:
        return gateway_delete_entry(data.tag, &data.status, gateway_commands,
                                    gateway_responses);
    default:
        return 1;
    }
}

void gateway(gateway_registers& r,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)
{
    static bool axilite_gateway_done;
#pragma HLS pipeline enable_flush ii=2
DO_PRAGMA_SYN(HLS data_pack variable=r.cmd)
    if (r.cmd.go && !axilite_gateway_done) {
        int res = firewall_rpc(r.cmd.addr, r.data, gateway_commands,
                               gateway_responses);
        if (res != 1) {
            axilite_gateway_done = true;
            r.done = 1;
        }
    } else if (!r.cmd.go && axilite_gateway_done) {
        axilite_gateway_done = false;
        r.done = 0;
    }
}

void map_metadata_to_hash_lookup(metadata_stream& in,
        hls::stream<hash_tag> &out)
{
#pragma HLS pipeline ii=3
    if (in.empty() || out.full())
        return;

    metadata m;
    in.read_nb(m);
    out.write_nb(hash_tag(m.ip_source, m.ip_dest, m.udp_source, m.udp_dest));
}

void merge_hash_results(lookup_result_stream& results,
                        metadata_stream& metadata_in, bool_stream &classify_out)
{
#pragma HLS pipeline ii=3
    if (results.empty() || metadata_in.empty() || classify_out.full())
        return;
    
    ntl::maybe<std::pair<uint32_t, ap_uint<1> > > val = results.read();
    metadata m = metadata_in.read();
     
    bool forward = !valid_udp(m) ||
        (val.valid() ? val.value().second : ap_uint<1>(0));
    classify_out.write(forward);
}

void firewall_step(axi_data_stream& in, axi_data_stream& data_out, bool_stream& classify_out, gateway_registers& g)
{
#pragma HLS inline
#pragma HLS stream variable=invalid_udp depth=16
    static bool_stream invalid_udp;
    static bool_stream result_with_default;

    static axi_data_stream dup_to_parse;
    static metadata_stream parse_to_dup, dup_metadata_to_hash, dup_metadata_to_invalid;

    static hls::stream<gateway_command> gateway_commands;
    static hls::stream<gateway_response> gateway_responses;
    static tag_stream lookups;
    static lookup_result_stream results;

    gateway(g, gateway_commands, gateway_responses);

    dup(in, dup_to_parse, data_out);
    parser(dup_to_parse, parse_to_dup);
    dup(parse_to_dup, dup_metadata_to_hash, dup_metadata_to_invalid);

    map_metadata_to_hash_lookup(dup_metadata_to_hash, lookups);

    hash_table(lookups, results, gateway_commands, gateway_responses);

    merge_hash_results(results, dup_metadata_to_invalid, classify_out);
}

void firewall_top(hls::stream<ntl::raw_axi_data>& in,
                  hls::stream<ntl::raw_axi_data>& data_out,
                  bool_stream& classify_out_stream, gateway_registers& g)
{
#pragma HLS dataflow
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=in
#pragma HLS interface axis port=data_out

#pragma HLS interface s_axilite port=g.cmd offset=0x100
#pragma HLS interface s_axilite port=g.data offset=0x118
#pragma HLS interface s_axilite port=g.done offset=0xfc

    static axi_data_stream in_fifo, out_fifo;
    ntl::link(in, in_fifo);
#pragma HLS stream variable=out_fifo depth=16
    ntl::link(out_fifo, data_out);
    firewall_step(in_fifo, out_fifo, classify_out_stream, g);
}
