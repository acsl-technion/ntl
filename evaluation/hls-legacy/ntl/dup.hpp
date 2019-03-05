#pragma once

#include "hls_stream.h"

template <typename T>
void dup(hls::stream<T>& in, hls::stream<T>& out1, hls::stream<T>& out2)
{
#pragma HLS pipeline
    if (in.empty())
        return;

    if (out1.full() || out2.full())
        return;

    const T flit = in.read();
    out1.write(flit);
    out2.write(flit);
}
