#include "ntl/map.hpp"
#include "ntl/cache.hpp"
#include "parser.hpp"
#include "firewall.hpp"

#include <functional>

#include <boost/operators.hpp>

typedef ntl::stream<ap_uint<1> > bool_stream;

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
                range<8, 12>(d),
                range<4, 8>(d),
                range<2, 4>(d),
                range<0, 2>(d)};
        }
    };
}

std::size_t hash_value(hash_tag tag)
{
#pragma HLS pipeline enable_flush ii=2
    std::size_t seed = 0;

    boost::hash_combine(seed, tag.ip_source.to_int());
    boost::hash_combine(seed, tag.ip_dest.to_int());
    boost::hash_combine(seed, tag.udp_source.to_short());
    boost::hash_combine(seed, tag.udp_dest.to_short());

    return seed;
}

typedef ntl::hash_table_wrapper<hash_tag, ap_uint<1>, 1024> hash_t;

struct gateway_data
{
        hash_tag tag;
        ap_uint<1> result;
        int status;
};

typedef ntl::gateway_registers<gateway_data> gateway_registers;

class firewall : public ntl::gateway_impl<firewall, gateway_data>
{
public:
    void step(axi_data_stream& in, axi_data_stream& data_out, bool_stream& classify_out, gateway_registers& g)
    {
#pragma HLS inline
        gateway(this, g);

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
            return val.valid() ? std::get<1>(val.value()) : ap_uint<1>(1);
        });

        merge_hash_results.step(std::logical_or<ap_uint<1> >(),
                                result_with_default, invalid_udp);

        link(merge_hash_results.out, classify_out);
        link(dup_data._streams[1], data_out);
    }

    int rpc(int addr, gateway_data& data)
    {
        switch (addr) {
        case FIREWALL_ADD:
            return hash.gateway_add_entry(std::make_tuple(data.tag, data.result), &data.status);
        case FIREWALL_DEL:
            return hash.gateway_delete_entry(data.tag, &data.status);
        default:
            return GW_FAIL;
        }
    }
private:
    ntl::dup<axi_data, 2> dup_data;
    ntl::dup<metadata, 2> dup_metadata;
    parser parse;
    hash_t hash;
    bool_stream invalid_udp;
    bool_stream result_with_default;
    ntl::zip_with<ap_uint<1>, ap_uint<1>, ap_uint<1> > merge_hash_results;
};

void firewall_top(axi_data_stream& in, axi_data_stream& data_out, bool_stream& classify_out, gateway_registers& g)
{
#pragma HLS dataflow
    GATEWAY_OFFSET(g, 0x100, 0x118, 0xfc)
    static firewall f;
    f.step(in, data_out, classify_out, g);
}
