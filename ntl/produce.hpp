#pragma once

#include "constant.hpp"

namespace ntl {

    template <typename Stream>
    void produce(Stream& out, bool enabled = true)
    {
#pragma HLS inline region
        typedef typename stream_element<Stream>::type stream_element_t;
        constant(stream_element_t(0)).step(out, enabled);
    }
}
