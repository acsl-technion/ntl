//
// Copyright (c) 2016-2019 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
#pragma HLS pipeline enable_flush
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
