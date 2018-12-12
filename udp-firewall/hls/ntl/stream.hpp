#pragma once

#include <hls_stream.h>

namespace ntl {
    template <typename Stream>
    struct stream_element;

    template <typename T>
    struct stream_element<hls::stream<T> >
    {
        typedef T type;
    };

    struct ap_fifo_tag {};
    struct axi_stream_input_tag {};
    struct axi_stream_output_tag {};

    template <typename T, typename Kind = ap_fifo_tag> class stream;

    template <typename T>
    class stream<T, ap_fifo_tag>
    {
    public:
        stream() = default;
        explicit stream(const char *name) : _stream(name) {}

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            _stream.write_nb(u);
        }

        T read()
        {
#pragma HLS inline
            T t;
            _stream.read_nb(t);
            return t;
        }

        bool empty()
        {
#pragma HLS inline
            return _stream.empty();
        }

        bool full()
        {
#pragma HLS inline
            return _stream.full();
        }

        hls::stream<T> _stream;
    };

    template <typename T>
    class stream<T, axi_stream_input_tag>
    {
    public:
        stream() = default;
        explicit stream(const char *name) : _stream(name) {}

        T read()
        {
#pragma HLS inline
            return _stream.read();
        }

        bool empty()
        {
#pragma HLS inline
            return _stream.empty();
        }

        hls::stream<T> _stream;
    };

    template <typename T>
    class stream<T, axi_stream_output_tag>
    {
    public:
        stream() = default;
        explicit stream(const char *name) : _stream(name) {}

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            _stream.write(u);
        }

        bool full()
        {
#pragma HLS inline
            return false;
        }

        hls::stream<T> _stream;
    };

    template <typename T, typename Kind>
    struct stream_element<stream<T, Kind> >
    {
        typedef T type;
    };
}
