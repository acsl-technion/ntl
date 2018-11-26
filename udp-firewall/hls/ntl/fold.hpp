#pragma once

#include "last.hpp"

namespace ntl {

    template <typename In, typename Out, bool out_every_flit>
    class fold
    {
    public:
        typedef stream<In> in_t;
        typedef stream<Out> out_t;
        out_t out;

        explicit fold(Out&& initial) : _initial(initial) {}

        template <typename Func>
        void step(in_t& in, Func&& f)
        {
#pragma HLS inline region
            if (in.empty() || out.full())
                return;

            auto flit = in.read();
            auto next = f(_current, flit);
            if (out_every_flit || last(flit))
                out.write(next);
            _current = last(flit) ? _initial : next;
        }
    private:
        const Out _initial;
        Out _current;
    };
}
