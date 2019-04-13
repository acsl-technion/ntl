#pragma once

namespace ntl {

    class consume_stream
    {
    public:
        template <typename Stream>
        void step(Stream& in, bool enabled = true)
        {
#pragma HLS pipeline
            if (!enabled)
                return;

            if (in.empty())
                return;
                
            in.read();
        }
    };

    template <typename Stream>
    void consume(Stream& in, bool enabled = true)
    {
        consume_stream().step(in, enabled);
    }
}
