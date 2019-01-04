#pragma once

#include "ntl/dup.hpp"
#include "ntl/link.hpp"
#include "ntl/axi_data.hpp"

#include <net/ethernet.h>
#include <netinet/in.h>

typedef hls::stream<ntl::axi_data> axi_data_stream;

struct metadata {
    ap_uint<32> ip_source, ip_dest;
    ap_uint<16> ether_type, udp_source, udp_dest;
    ap_uint<8> ip_protocol;

    metadata() :
        ip_source(0), ip_dest(0), ether_type(0), udp_source(0), udp_dest(0),
        ip_protocol(0)
    {}

    bool valid_ip() const
    {
        return ether_type == ETHERTYPE_IP;
    }

    bool valid_udp() const
    {
        return valid_ip() && ip_protocol == IPPROTO_UDP;
    }
};

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
    static ap_uint<16> index = 0;
    static metadata ret;

    ntl::axi_data flit;

    if (in.empty() || out.full())
        return;

    if (index == 0) {
        ret.ether_type = range<12, 14>(flit.data);
        ret.ip_protocol = range<23, 24>(flit.data);
        ret.ip_source = range<26, 30>(flit.data);
        ret.ip_dest(31, 16) = range<30, 32>(flit.data);
    } else if (index == 1) {
        ret.ip_dest(15, 0) = range<0, 2>(flit.data);
        ret.udp_source = range<2, 4>(flit.data);
        ret.udp_dest = range<4, 6>(flit.data);
    }
    if (flit.last) {
        out.write(ret);
        ret = metadata();
        index = 0;
    } else {
        index++;
    }
}

/*
template <unsigned start, unsigned end>
class extract_header
{
public:
    extract_header() : _flit_count(0) {}

    static const unsigned len = end - start;
    typedef ap_uint<8 * len> field_t;
    typedef ntl::stream<field_t> field_stream;
    field_stream out;

    void step(axi_data_stream& in)
    {
        static_assert(31 / 32 == (30 / 32), "No support for fields that span multiple flits at the moment.");
        static_assert((end - 1) / 32 == (start / 32), "No support for fields that span multiple flits at the moment.");

        if (in.empty())
            return;

        if (start / 32 != _flit_count++) {
            in.read();
        } else {
            if (out.full())
                return;

            auto flit = in.read();
            out.write(flit.data(8 * end - 1, 8 * start));
            if (flit.last)
                _flit_count = 0;
        }
    }

private:
    ap_uint<16> _flit_count;
};
*/
