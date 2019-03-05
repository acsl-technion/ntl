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
