#pragma once

#include <tuple>

namespace ntl {

    template <typename Out, typename ...Args>
    class zip
    {
    public:
        typedef stream<Out> stream_t;
        stream_t out;

        void step(stream<Args>&... in)
        {
#pragma HLS pipeline
            if (out.full())
                return;

            bool empties[] = {in.empty()...};
#pragma HLS array_partition variable=empties complete
            check_empty: for (auto empty : empties) {
#pragma HLS unroll
                if (empty)
                    return;
            }

            out.write(std::make_tuple(in.read()...));
        }
    };

    template <typename Out, typename ...Args>
    class zip_with
    {
    public:
        typedef stream<Out> stream_t;
        stream_t out;

        template <typename Func>
        void step(Func&& f, stream<Args>&... in)
        {
#pragma HLS inline region
            if (out.full())
                return;

            auto empties = {in.empty()...};
            for (auto empty : empties) {
                if (empty)
                    return;
            }

            out.write(f(in.read()...));
        }
    };
}
