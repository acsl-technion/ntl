#pragma once

#include <hls_stream.h>

namespace ntl {
    struct ap_fifo_tag {};
    struct axi_stream_input_tag {};
    struct axi_stream_output_tag {};

    namespace detail {
        template <typename T>
        class stream_common {
        public:
            stream_common() : _stream() {}

            bool empty()
            {
    #pragma HLS inline
                return _stream.empty();
            }

        protected:
            hls::stream<T> _stream;
        };
    }

    template <typename T, typename Kind = ap_fifo_tag> class stream;

    template <typename T>
    class stream<T, ap_fifo_tag> : public detail::stream_common<T>
    {
    public:
        typedef detail::stream_common<T> base;

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            base::_stream.write_nb(u);
        }

        T read()
        {
#pragma HLS inline
            T t;
            base::_stream.read_nb(t);
            return t;
        }

        bool full()
        {
#pragma HLS inline
            return base::_stream.full();
        }
    };

    template <typename T>
    class stream<T, axi_stream_input_tag> : public detail::stream_common<T>
    {
    public:
        typedef detail::stream_common<T> base;

        T read()
        {
#pragma HLS inline
            return base::_stream.read();
        }
    };

    template <typename T>
    class stream<T, axi_stream_output_tag> : public detail::stream_common<T>
    {
    public:
        typedef detail::stream_common<T> base;

        template <typename U>
        void write(const U& u)
        {
#pragma HLS inline
            base::_stream.write(u);
        }

        bool full()
        {
#pragma HLS inline
            return false;
        }
    };
}
