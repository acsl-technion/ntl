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
