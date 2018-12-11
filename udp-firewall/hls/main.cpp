#include "parser.hpp"
#include "firewall.hpp"
#include "ntl/macros.hpp"
#include <pcap/pcap.h>
#include <gtest/gtest.h>

struct packet_handler_context {
    axi_data_stream& stream;
    int count;
    int range_start;
    int range_end;

    explicit packet_handler_context(axi_data_stream& stream) :
        stream(stream), count(0) {}
};

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    auto context = reinterpret_cast<packet_handler_context*>(user);
    axi_data_stream& stream = context->stream;
    const int b = 32;

    if (h->caplen != h->len)
        return;

    if (context->count < context->range_start || context->count >= context->range_end)
        goto end;

    for (unsigned word = 0; word < ALIGN(h->len, b); word += b) {
        axi_data input(0, 0xffffffff, false);
        for (unsigned byte = 0; byte < b && word + byte < h->len; ++byte)
            input.data(input.data.width - 1 - 8 * byte, input.data.width - 8 - 8 * byte) = bytes[word + byte];
        if ((word + b) >= h->len) {
            input.keep = axi_data::keep_bytes(h->len - word - b);
            input.last = true;
        }

        stream.write(input);
    }
end:
    ++context->count;
}

int read_pcap(
    const std::string& filename, axi_data_stream& stream,
    int range_start, int range_end)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *file = pcap_open_offline(filename.c_str(), errbuf);

    if (!file) {
        fprintf(stderr, "%s\n", errbuf);
        return -1;
    }

    auto context = packet_handler_context(stream);
    context.range_start = range_start; context.range_end = range_end;
    int ret = pcap_loop(file, 0, &packet_handler, (u_char *)&context);
    if (ret == -1) {
        perror("pcap_loop returned error");
        return -1;
    }

    pcap_close(file);

    return context.count;
}

int write_pcap(FILE* file, axi_data_stream& stream, bool expected_lossy)
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
        axi_data w = stream.read();

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

int main()
{
    firewall f;
    axi_data_stream in_fifo, out_fifo;
    bool_stream classify_out;
    gateway_registers regs;

    f.step(in_fifo, out_fifo, classify_out, regs);
    return 0;
}
