#pragma once

#include <array>
#include <algorithm>

namespace ntl {

    template <typename T, unsigned n>
    class dup
    {
    public:
        std::array<stream<T>, n> _streams;

        template <typename InputStream>
        void step(InputStream& in)
        {
#pragma HLS pipeline enable_flush
            if (in.empty())
                return;

            for (auto& out : _streams) {
                if (out.full())
                    return;
            }

            auto flit = in.read();
            for (auto& out : _streams) {
                out.write(flit);
            }
        }
    };
}
