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
