#pragma once

#include "ntl/stream.hpp"
#include "ntl/dup.hpp"
#include "ntl/zip.hpp"
#include "ntl/consume.hpp"
#include "ntl/fold.hpp"
#include "ntl/link.hpp"
#include "axi_data.hpp"

typedef ntl::stream<axi_data> axi_data_stream;

struct metadata {
    ap_uint<32> ip_source, ip_dest;
    ap_uint<16> ether_type, udp_source, udp_dest;
    ap_uint<8> ip_protocol;
};

typedef ntl::stream<metadata> metadata_stream;

template <typename T, typename Counter = ap_uint<16> >
class counter : public ntl::fold<T, Counter, true>
{
public:
    typedef ntl::fold<T, Counter, true> base;
    typedef typename base::in_t in_t;
    counter() : base(0) {}

    void step(in_t& in)
    {
#pragma HLS pipeline
        base::step(in, [](const Counter& cnt, const T& t) {
            return Counter(cnt + 1);
        });
    }
};

template <typename T, typename Counter = ap_uint<16> >
class enumerate
{
public:
    typedef ntl::stream<T> in_t;
    typedef std::tuple<Counter, T> tuple_t;
    typedef ntl::stream<tuple_t> out_t;
    out_t out;

    void step(in_t& in)
    {
#pragma HLS inline
        dup.step(in);
        _counter.step(dup._streams[0]);
        zip.step(_counter.out, dup._streams[1]);
        ntl::link(zip.out, out);
    }
private:
    ntl::dup<T, 2> dup;
    counter<T, Counter> _counter;
    ntl::zip<tuple_t, Counter, T> zip;
};

typedef std::tuple<ap_uint<16>, axi_data> numbered_data;

namespace ntl {
    template <>
    inline bool last<numbered_data>(const numbered_data& flit)
    {
        return std::get<1>(flit).last;
    }
}

template <unsigned start, unsigned end, typename T>
ap_uint<8 * (end - start)> range(T val)
{
    static_assert(end > start, "Invalid range.");

    return val(8 * end - 1, 8 * start);
}

class extract_metadata : public ntl::fold<numbered_data, metadata, false>
{
public:
    typedef ntl::fold<numbered_data, metadata, false> base;
    typedef typename base::in_t in_t;
    extract_metadata() : base(metadata{}) {}

    void step(in_t& in)
    {
#pragma HLS pipeline
        base::step(in, [](const metadata& cur, const numbered_data& num_data) -> metadata {
            metadata ret = cur;
            axi_data flit;
            ap_uint<16> index;

            std::tie(index, flit) = num_data;

            if (index == 0) {
                ret.ether_type = range<12, 14>(flit.data);
                ret.ip_protocol = range<21, 22>(flit.data);
                ret.ip_source = range<26, 30>(flit.data);
                ret.ip_dest(31, 16) = range<30, 32>(flit.data);
            } else if (index == 1) {
                ret.ip_dest(15, 0) = range<0, 2>(flit.data);
                ret.udp_source = range<2, 4>(flit.data);
                ret.udp_dest = range<4, 6>(flit.data);
            }
            return ret;
        });
    }
};

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

class parser {
public:
    metadata_stream out;

    void step(axi_data_stream& in)
    {
#pragma HLS dataflow
        _enum.step(in);
        _extract.step(_enum.out);
        ntl::link(_extract.out, out);
    }

private:
    enumerate<axi_data> _enum;
    extract_metadata _extract;
};
