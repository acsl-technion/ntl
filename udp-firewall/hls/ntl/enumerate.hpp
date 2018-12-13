#pragma once

#include "counter.hpp"
#include "dup.hpp"
#include "zip.hpp"

namespace ntl {

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

}
