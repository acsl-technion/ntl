#pragma once

#include "counter.hpp"
#include "dup.hpp"
#include "zip.hpp"

namespace ntl {

    template <typename T, typename Counter = ap_uint<16> >
    class enumerate
    {
    public:
        typedef std::tuple<Counter, T> tuple_t;
        typedef stream<tuple_t> out_t;
        out_t out;

        template <typename InputStream>
        void step(InputStream& in)
        {
    #pragma HLS inline
            dup.step(in);
            _counter.step(dup._streams[0]);
            zip.step(_counter.out, dup._streams[1]);
            link(zip.out, out);
        }
    private:
        ntl::dup<T, 2> dup;
        counter<T, Counter> _counter;
        ntl::zip<tuple_t, Counter, T> zip;
    };

    template <typename T>
    class enum_first : public enumerate<T, maxed_int<2, 2>>
    {
    };
}
