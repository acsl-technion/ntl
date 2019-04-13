#pragma once

namespace ntl {
    template <typename T>
    class constant_stream
    {
    public:
        constant_stream(const T& value) : value(value) {}

        template <typename Stream>
        void step(Stream& out, bool enabled = true)
        {
            if (enabled && !out.full())
                out.write(value);
        }

    private:
        T value;
    };

    template <typename T>
    constant_stream<T> constant(const T& value)
    {
        return constant_stream<T>(value);
    }
}
