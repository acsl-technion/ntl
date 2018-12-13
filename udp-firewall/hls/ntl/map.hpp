#pragma once

#include "stream.hpp"

namespace ntl {

    template <typename InputStream, typename OutputStream, typename Func>
    void map(InputStream& in, OutputStream& out, Func&& f)
    {
#pragma HLS pipeline
        if (in.empty() || out.full())
            return;

        out.write(f(in.read()));
    }
}
