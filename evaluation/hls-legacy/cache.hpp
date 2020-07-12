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

#pragma once

#include "ntl-legacy/hash.hpp"

#include "firewall.hpp"

#include <hls_stream.h>
#include <ap_int.h>

#include "ntl-legacy/macros.hpp"

// #undef CACHE_ENABLE_DEBUG_COMMANDS

typedef hash_tag tag_type;
typedef ap_uint<1> mapped_type;
static const int Size = 1024;
static const int Log_Size = 10;

typedef ap_uint<Log_Size> index_t;

typedef ntl_legacy::hash<tag_type, mapped_type, Size, 1, false> hash_table_t;
typedef typename hash_table_t::value_type value_type;

typedef hls::stream<value_type> value_stream;
typedef hls::stream<tag_type> tag_stream;
typedef hls::stream<ntl_legacy::maybe<std::pair<uint32_t, mapped_type> > > lookup_result_stream;

enum gateway_command_enum {
    HASH_INSERT,
    HASH_ERASE,
    HASH_READ,
    HASH_WRITE,
};

typedef ntl_legacy::maybe<value_type> maybe_value_t;

struct gateway_command
{
    gateway_command_enum cmd;
    index_t index;
    maybe_value_t value;
};
typedef std::pair<index_t, maybe_value_t> gateway_response;

void hash_table(tag_stream& lookups, lookup_result_stream& results,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)
{
    static hash_table_t _table;

#pragma HLS pipeline enable_flush ii=3
    /* Gateway access are highest priority as they are more rare and lower
     * throughput */
    gateway_command cmd;
    if (gateway_commands.read_nb(cmd)) {
        gateway_response resp;

        switch (cmd.cmd) {
        case HASH_ERASE: {
            tag_type tag = cmd.value.value().first;
            bool result = _table.erase(tag);
            resp.second = ntl_legacy::make_maybe(result, cmd.value.value());
            break;
        }
        case HASH_INSERT: {
            value_type value = cmd.value.value();
            ntl_legacy::maybe<uint64_t> result = _table.insert(value.first, value.second);
            /* Return zero for failure, 1 + index otherwise */
            resp.first = result ? result.value() + 1 : 0;
            break;
        }
#ifdef CACHE_ENABLE_DEBUG_COMMANDS
        case HASH_WRITE: {
            const bool valid = cmd.value.valid();
            const tag_type tag = std::get<0>(cmd.value.value());
            const mapped_type value = std::get<1>(cmd.value.value());
            _table.set_entry(cmd.index, valid, tag, value);
            break;
        }
        case HASH_READ: {
            bool valid = _table.get_valid(cmd.index);
            tag_type tag = _table.get_tag(cmd.index);
            mapped_type value = _table.get_value(cmd.index);
            std::get<1>(resp) = ntl_legacy::make_maybe(valid, std::make_pair(tag, value));
            break;
        }
#endif
        }
        gateway_responses.write(resp);
        return;
    }

    if (!lookups.empty()) {
        tag_type tag = lookups.read();
        size_t index;
        ntl_legacy::maybe<ap_uint<1> > result = _table.find(tag, index);
        ntl_legacy::maybe<std::pair<uint32_t, mapped_type> > returned_results =
            ntl_legacy::make_maybe(result.valid(),
            std::make_pair(result.valid() ? uint32_t(index + 1) : 0,
                           result.value()));
        results.write(returned_results);
    }

}

/* Command from the gateway */
int gateway_execute_command(const gateway_command& cmd, gateway_response& resp,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)

{
#pragma HLS inline
    static bool gateway_command_sent = false;

    if (!gateway_command_sent) {
        if (!gateway_commands.write_nb(cmd))
            return 1;

        gateway_command_sent = true;
        return 1;
    } else {
        if (!gateway_responses.read_nb(resp))
            return 1;

        gateway_command_sent = false;
        return 0;
    }
}

/* Insert a new entry from the gateway. Returns GW_DONE when completed,
 * *result == 1 if successful. */
int gateway_add_entry(const value_type& value, int *result,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)
{
#pragma HLS inline
    gateway_command cmd = {
        HASH_INSERT,
        0,
        value
    };
    gateway_response resp;

    int ret = gateway_execute_command(cmd, resp, gateway_commands,
                                      gateway_responses);
    if (ret == 0)
        *result = resp.first;

    return ret;
}

/* Remove an entry from the gateway. Returns GW_DONE when completed,
 * *result == 1 if successful. */
int gateway_delete_entry(const typename hash_table_t::tag_type& tag, int *result,
    hls::stream<gateway_command>& gateway_commands,
    hls::stream<gateway_response>& gateway_responses)
{
#pragma HLS inline
    gateway_command cmd = {
        HASH_ERASE,
        0,
        std::make_pair(tag, mapped_type())
    };
    gateway_response resp;

    int ret = gateway_execute_command(cmd, resp, gateway_commands,
                                      gateway_responses);
    if (ret == 0)
        *result = resp.second.valid();

    return ret;
}

