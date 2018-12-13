#pragma once

#include "fold.hpp"

#include <ap_int.h>

namespace ntl {
    template <typename T, typename Counter = ap_uint<16> >
    class counter : public ntl::fold<T, Counter, true>
    {
    public:
        typedef ntl::fold<T, Counter, true> base;
        typedef typename base::in_t in_t;
        counter() : base(-1) {}

        void step(in_t& in)
        {
    #pragma HLS pipeline
            base::step(in, [](const Counter& cnt, const T& t) {
                return Counter(cnt + 1);
            });
        }
    };
}
