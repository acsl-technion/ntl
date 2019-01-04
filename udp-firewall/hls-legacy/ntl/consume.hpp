#pragma once

namespace ntl {

    template <typename T>
    void consume(hls::stream<T>& in)
    {
#pragma HLS pipeline
        if (in.empty())
            return;
            
        in.read();
    }
}
