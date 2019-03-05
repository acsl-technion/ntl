#pragma once

namespace ntl_legacy {

    template <typename InputStream, typename OutputStream>
    void link(InputStream& in, OutputStream& out)
    {
#pragma HLS pipeline
        if (in.empty() || out.full())
            return;

        out.write(in.read());
    }
}
