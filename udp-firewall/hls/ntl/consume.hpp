#pragma once

#include "stream.hpp"

namespace ntl {

    template <typename T>
    class consume
    {
    public:
        void step(stream<T>& in)
        {
#pragma HLS pipeline
            if (in.empty())
                return;
                
            in.read();
        }
    };
}
