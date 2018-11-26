#include "ntl/map.hpp"
#include "ntl/consume.hpp"
#include "parser.hpp"

typedef ntl::stream<bool> bool_stream;

class firewall
{
public:
    void step(axi_data_stream& in, bool_stream& out)
    {
#pragma HLS inline
        dup.step(in);
        parse.step(dup._streams[0]);
        // dropper.step(dup._streams[1], );
        ntl::consume<axi_data>().step(dup._streams[1]);
        ntl::map(parse.out, out, [](const metadata& m) {
            return m.valid_udp();
        });
    }
private:
    ntl::dup<axi_data, 2> dup;
    parser parse;
    // drop_or_pass dropper;
};

void firewall_top(axi_data_stream& in, bool_stream& out)
{
#pragma HLS dataflow
    static firewall f;
    f.step(in, out);
}
