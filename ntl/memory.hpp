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

#include "macros.hpp"
#include "stream.hpp"
#include "produce.hpp"
#include "consume.hpp"
#include "link.hpp"

namespace ntl {

    template <size_t _interface_width, typename Kind>
    class memory {
    public:
        enum {
            interface_width = _interface_width,
        };
        typedef ap_uint<512> value_t;
        typedef ap_uint<interface_width - 6> index_t;

        /* TODO pass other auxilary signals */

        stream<index_t, Kind> ar;
        stream<value_t, Kind> r;
        stream<index_t, Kind> aw;
        stream<value_t, Kind> w;
        stream<bool, Kind> b;

        void write(index_t index, value_t value)
        {
#pragma HLS inline
            aw.write(index);
            w.write(value);
        }

        void post_read(index_t index)
        {
#pragma HLS inline
            ar.write(index);
        }

        bool has_write_response()
        {
#pragma HLS inline
            return !b.empty();
        }

        bool get_write_response()
        {
#pragma HLS inline
            return b.read();
        }

        bool has_read_response()
        {
#pragma HLS inline
            return !r.empty();
        }

        value_t get_read_response()
        {
#pragma HLS inline
            return r.read();
        }
    };

    template <size_t interface_width, typename Kind1, typename Kind2>
    static inline void link(memory<interface_width, Kind1>& in, memory<interface_width, Kind2>& out)
    {
#pragma HLS inline
        link(out.aw, in.aw);
        link(out.w, in.w);
        link(out.ar, in.ar);
        link(in.b, out.b);
        link(in.r, out.r);
    }

    /* In cases the memory interface is not used, this function convinces HLS
     * that it is used, and set the right stream directions. */
    class memory_unused
    {
    public:
        memory_unused() : dummy_update("dummy_update") {}

        template <size_t interface_width, typename Kind>
        void step(memory<interface_width, Kind>& m)
        {
    #pragma HLS inline
            bool dummy = false;

            if (!dummy_update.full())
                dummy_update.write(false);

            if (!dummy_update.empty())
                dummy = dummy_update.read();

            ntl::produce(m.ar, dummy);
            ntl::produce(m.aw, dummy);
            ntl::produce(m.w, dummy);
            ntl::consume(m.r, dummy);
            ntl::consume(m.b, dummy);
        }

    private:
        /* Dummy boolean to make it easier to define unused HLS stream direction. Pass it
         * to produce/consume function to trick HLS into thinking they are used. */
        stream<bool> dummy_update;
    };
}

#define NTL_MEMORY_INTERFACE_PRAGMA(memory) \
    DO_PRAGMA_SYN(HLS interface axis port=&memory.ar) \
    DO_PRAGMA_SYN(HLS interface axis port=&memory.r) \
    DO_PRAGMA_SYN(HLS interface axis port=&memory.aw) \
    DO_PRAGMA_SYN(HLS interface axis port=&memory.w) \
    DO_PRAGMA_SYN(HLS interface axis port=&memory.b)
