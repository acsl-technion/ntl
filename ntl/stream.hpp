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

#include <hls_stream.h>

namespace ntl {
    template <typename Stream>
    struct stream_element;

    template <typename T>
    struct stream_element<hls::stream<T> >
    {
        typedef T type;
    };

    struct ap_fifo_tag {};
    struct axi_stream_tag {};

    template <typename T, typename Kind = ap_fifo_tag> class stream;

    template <typename T>
    class stream<T, ap_fifo_tag>
    {
    public:
        stream() = default;
        explicit stream(const char *name) : _stream(name) {}

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            _stream.write_nb(u);
        }

        T read()
        {
#pragma HLS inline
            T t;
            _stream.read_nb(t);
            return t;
        }

        bool empty()
        {
#pragma HLS inline
            return _stream.empty();
        }

        bool full()
        {
#pragma HLS inline
            return _stream.full();
        }

        hls::stream<T> _stream;
    };

    template <typename T>
    class stream<T, axi_stream_tag>
    {
    public:
        stream() = default;
        explicit stream(const char *name) : _stream(name) {}

        T read()
        {
#pragma HLS inline
            return _stream.read();
        }

        bool empty()
        {
#pragma HLS inline
            return _stream.empty();
        }

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            _stream.write(u);
        }

        bool full()
        {
#pragma HLS inline
            return false;
        }

        hls::stream<T> _stream;
    };

    template <typename T, typename Kind>
    struct stream_element<stream<T, Kind> >
    {
        typedef T type;
    };
}
