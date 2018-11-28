#pragma once

#include <tuple>

namespace ntl {

    template <typename InputStream, typename OutputStream>
    void link(InputStream& in, OutputStream& out)
    {
#pragma HLS pipeline
        typedef typename stream_element<OutputStream>::type out_element_type;

        if (in.empty() || out.full())
            return;

        out.write(out_element_type(in.read()));
    }
}
