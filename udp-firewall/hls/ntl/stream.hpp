#pragma once

#include <hls_stream.h>

namespace ntl {
    template <typename T, bool axi_write = false, bool axi_read = false>
    class stream
    {
    public:
        stream() : _stream() {}

        template <typename U>
        void write(const U& u)
        {
            if (axi_write)
                _stream.write(u);
            else
                _stream.write_nb(u);
        }

        T read()
        {
            T t;
            if (axi_read) {
                t = _stream.read();
            } else {
                _stream.read_nb(t);
            }
            return t;
        }

        bool empty()
        {
            return _stream.empty();
        }

        bool full()
        {
            if (axi_write)
                return false;
            else
                return _stream.full();
        }
    private:
        hls::stream<T> _stream;
    };
}
