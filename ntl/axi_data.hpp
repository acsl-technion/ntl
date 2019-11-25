//
// Copyright (c) 2016-2018 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
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

#include <cstddef>
#include <ap_int.h>
#include <boost/operators.hpp>

namespace ntl {
    struct raw_axi_data {
        ap_uint<256> data;
        ap_uint<32> keep;
        ap_uint<1> last;
    };

    struct axi_data : public boost::equality_comparable<axi_data> {
        static const int data_bits = 256;
        static const int data_bytes = data_bits / 8;

        ap_uint<data_bits> data;
        ap_uint<data_bytes> keep;
        ap_uint<1> last;

        axi_data() {}
        axi_data(const ap_uint<data_bits>& data, const ap_uint<data_bytes>& keep, bool last) :
            data(data), keep(keep), last(last) {}
        axi_data(const axi_data&) = default;
        axi_data(const raw_axi_data& o) :
            data(o.data), keep(o.keep), last(o.last) {}

        static ap_uint<data_bytes> keep_bytes(const ap_uint<6>& valid_bytes)
        {
            return 0xffffffff ^ ((1 << (data_bytes - valid_bytes)) - 1);
        }

        void set_data(const char *d, const ap_uint<6>& valid_bytes)
        {
            keep = keep_bytes(valid_bytes);
            for (int byte = 0; byte < data_bytes; ++byte) {
    #pragma HLS unroll
                const char data_word = (byte < valid_bytes) ? d[byte] : 0;
                data(data.width - 1 - 8 * byte, data.width - 8 - 8 * byte) = data_word;
            }
        }

        int get_data(char *d) const
        {
            for (int byte = 0; byte < data_bytes; ++byte) {
    #pragma HLS unroll
                const uint8_t cur = data(data.width - 1 - 8 * byte, data.width - 8 - 8 * byte);
                if (keep[31 - byte])
                    d[byte] = cur;
                else
                    return byte;
            }

            return data_bytes;
        }

        bool operator ==(const axi_data& other) const { return data == other.data && keep == other.keep && last == other.last; }

        static const int width = data_bits + data_bytes + 1;

        axi_data(const ap_uint<width> d) :
            data(d(255 + data_bytes + 1, data_bytes + 1)),
            keep(d(data_bytes, 1)),
            last(d(0, 0))
        {}

        operator ap_uint<width>() const
        {
    #pragma HLS inline
            return (data, keep, last);
        }

        operator raw_axi_data() const
        {
    #pragma HLS inline
            return raw_axi_data{data, keep, last};
        }
    };

    static inline std::ostream& operator <<(std::ostream& out, const axi_data& d) {
        return out << "axi_data(" << std::hex << d.data << ", keep=" << d.keep << (d.last ? ", last)" : ")");
    }
}
