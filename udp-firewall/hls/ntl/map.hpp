#pragma once

#include "stream.hpp"

namespace ntl {

    template <typename T, typename U, typename Func>
    void map(stream<T>& in, stream<U>& out, Func&& f)
    {
#pragma HLS pipeline
        if (in.empty() || out.full())
            return;

        out.write(f(in.read()));
    }
}
