//
// Copyright (c) 2016-2019 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "parser.hpp"
#include "firewall.hpp"
#include "ntl-legacy/macros.hpp"
#include <pcap/pcap.h>
#include <gtest/gtest.h>

struct packet_handler_context {
    hls::stream<ntl_legacy::raw_axi_data>& stream;
    int count;
    int range_start;
    int range_end;

    explicit packet_handler_context(hls::stream<ntl_legacy::raw_axi_data>& stream) :
        stream(stream), count(0) {}
};

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    packet_handler_context *context = reinterpret_cast<packet_handler_context*>(user);
    hls::stream<ntl_legacy::raw_axi_data>& stream = context->stream;
    const int b = 32;

    if (h->caplen != h->len)
        return;

    if (context->count < context->range_start || context->count >= context->range_end)
        goto end;

    for (unsigned word = 0; word < ALIGN(h->len, b); word += b) {
        ntl_legacy::axi_data input(0, 0xffffffff, false);
        for (unsigned byte = 0; byte < b && word + byte < h->len; ++byte)
            input.data(input.data.width - 1 - 8 * byte, input.data.width - 8 - 8 * byte) = bytes[word + byte];
        if ((word + b) >= h->len) {
            input.keep = ntl_legacy::axi_data::keep_bytes(h->len - word - b);
            input.last = true;
        }

        stream.write(input);
    }
end:
    ++context->count;
}

int read_pcap(
    const std::string& filename, hls::stream<ntl_legacy::raw_axi_data>& stream,
    int range_start, int range_end)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *file = pcap_open_offline(filename.c_str(), errbuf);

    if (!file) {
        fprintf(stderr, "%s\n", errbuf);
        return -1;
    }

    packet_handler_context context(stream);
    context.range_start = range_start; context.range_end = range_end;
    int ret = pcap_loop(file, 0, &packet_handler, (u_char *)&context);
    if (ret == -1) {
        perror("pcap_loop returned error");
        return -1;
    }

    pcap_close(file);

    return context.count;
}

int write_pcap(FILE* file, hls::stream<ntl_legacy::raw_axi_data>& stream, bool_stream& classify_out)
{
    int ret;
    int count = 0;

    pcap_t *dead = pcap_open_dead(DLT_EN10MB, 65535);
    if (!dead) {
        perror("pcap_open_dead failed");
        return -1;
    }

    pcap_dumper_t *output = pcap_dump_fopen(dead, file);
    if (!output) {
        perror("pcap_dump_open failed");
        return -1;
    }

    u_char buffer[65535];
    pcap_pkthdr h = {};
    h.len = 0;

    while (!stream.empty()) {
        ntl_legacy::axi_data w = stream.read();

        for (int byte = 0; byte < w.data.width / 8; ++byte)
            buffer[h.len + byte] = w.data(w.data.width - 1 - byte * 8,
                                          w.data.width - 8 - byte * 8);
        h.len += w.data.width / 8;
        if (w.last) {
            for (int i = 0; i < w.data.width / 8; ++i)
                if (!w.keep(i, i))
                    --h.len;
                else
                    break;
            /* Minimum Ethernet packet length is 64 bytes including the FCS */
            h.caplen = h.len;
            assert(!classify_out.empty());
            if (!classify_out.read()) { // drop 
                pcap_dump((u_char *)output, &h, buffer);
                ++count;
            }
            h.len = 0;
        } else {
            EXPECT_EQ(~w.keep, 0);
        }
    }

    EXPECT_EQ(h.len, 0);
    EXPECT_TRUE(stream.empty());

    ret = pcap_dump_flush(output);
    if (ret) {
        perror("pcap_dump_flush returned error");
        return -1;
    }

    pcap_close(dead);
    fdatasync(fileno(file));

    return count;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s in.pcap out.pcap\n", argv[0]);
        return 0;
    }

    hls::stream<ntl_legacy::raw_axi_data> in_fifo("in_fifo"), out_fifo("out_fifo");
    bool_stream classify_out("classify_out");
    gateway_registers regs;
    regs.cmd.addr = FIREWALL_ADD; 
    regs.cmd.go = 1;
    regs.data.tag.ip_source = regs.data.tag.ip_dest = 0x7f000001;
    regs.data.tag.udp_source = 0x12;
    regs.data.tag.udp_dest = 0x0bad;
    regs.data.result = 1;
    for (int i = 0; i < 15; ++i)
        firewall_top(in_fifo, out_fifo, classify_out, regs);
    assert(regs.done);
    assert(regs.data.status);

    read_pcap(argv[1], in_fifo, 0, 1000000);

    for (int i = 0; i < 3000; ++i)
        firewall_top(in_fifo, out_fifo, classify_out, regs);

    FILE *out = fopen(argv[2], "w");
    write_pcap(out, out_fifo, classify_out);
    return 0;
}
