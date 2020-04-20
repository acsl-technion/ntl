#pragma once

namespace ntl {

    class consume_stream
    {
    public:
        template <typename Stream>
        void step(Stream& in, bool enabled = true)
        {
#pragma HLS pipeline enable_flush
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
#pragma HLS inline region
        consume_stream().step(in, enabled);
    }
}
