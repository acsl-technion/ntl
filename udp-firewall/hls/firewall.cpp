#include "ntl/consume.hpp"
#include "parser.hpp"

class firewall
{
public:
    void step(axi_data_stream& in, metadata_stream& out)
    {
#pragma HLS inline
        dup.step(in);
        parse.step(dup._streams[0]);
        // dropper.step(dup._streams[1], );
        ntl::consume<axi_data>().step(dup._streams[1]);
        ntl::link(parse.out, out);
    }
private:
    ntl::dup<axi_data, 2> dup;
    parser parse;
    // drop_or_pass dropper;
};

void firewall_top(axi_data_stream& in, metadata_stream& out)
{
#pragma HLS dataflow
    static firewall f;
    f.step(in, out);
}
