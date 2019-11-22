#pragma once

#include "fold.hpp"

#include <ap_int.h>
#include <boost/operators.hpp>

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
            base::step(in, [](Counter cnt, const T& t) {
                return ++cnt;
            });
        }
    };

    template <int width, int limit = (1 << width) - 1>
    class maxed_int : public ap_uint<width>, public boost::incrementable<maxed_int<width>>
    {
    public:
        typedef ap_uint<width> base;
        maxed_int() : base() {}
        maxed_int(const base& b) : base(b) {}
#define CTOR(TYPE)				\
        INLINE maxed_int(TYPE v) : base(v) {}
        CTOR(bool)
        CTOR(signed char)
        CTOR(unsigned char)
        CTOR(short)
        CTOR(unsigned short)
        CTOR(int)
        CTOR(unsigned int)
        CTOR(long)
        CTOR(unsigned long)
        CTOR(unsigned long long)
        CTOR(long long)
        CTOR(half)
        CTOR(float)
        CTOR(double)
        CTOR(const char*)
#undef CTOR

        maxed_int& operator++()
        {
            if (*this != limit)
                ap_uint<width>::operator++(*this);
            return *this;
        }
    };
}
