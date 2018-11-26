#pragma once

#include <tuple>

namespace ntl {

    template <typename T, typename U>
    void link(stream<T>& in, stream<U>& out)
    {
#pragma HLS pipeline
        if (in.empty() || out.full())
            return;

        out.write(in.read());
    }
}
