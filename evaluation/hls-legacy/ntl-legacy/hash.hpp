//
// Copyright (c) 2016-2018 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
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

#include "maybe.hpp"

namespace ntl_legacy {
    template <typename Tag, typename Value, unsigned Size, int max_hops = Size, bool insert_overrides = true>
    class hash
    {
    public:
        typedef Tag tag_type;
        typedef std::pair<Tag, Value> value_type;
        typedef uint64_t index_t;

        hash()
        {
#pragma HLS resource core=RAM_2P variable=tags
#pragma HLS resource core=RAM_2P variable=values
#pragma HLS resource core=RAM_2P variable=valid
            for (int i = 0; i < Size; ++i) {
                valid[i] = false;
                tags[i] = Tag();
                values[i] = Value();
            }
        }

        maybe<index_t> insert(const Tag& key, const Value& value)
        {
            maybe<index_t> index = lookup(h(key), key);

            if (!index.valid()) {
                return index;
            }

            if (valid[index.value()] && !insert_overrides)
                    return maybe<index_t>();

            tags[index.value()] = key;
            values[index.value()] = value;
            valid[index.value()] = true;

            return index;
        }

        bool erase(const Tag& k)
        {
            maybe<index_t> index = lookup(h(k), k);

            if (!index.valid() || !valid[index.value()] || tags[index.value()] != k) {
                return false;
            }

            valid[index.value()] = false;

            // fill the hole if needed
            index_t hash = index.value();
            bool found = false;

            for (int i = 1; i < max_hops; ++i) {
                if (found) continue;

                index_t cur = (hash + i) % Size;

                if (!valid[cur]) continue;
                if (h(tags[cur]) <= index.value()) {
                    tags[index.value()] = tags[cur];
                    values[index.value()] = values[cur];
                    valid[index.value()] = true;
                    valid[cur] = false;
                    found = true;
                    continue;
                }
            }

            return true;
        }

        maybe<Value> find(const Tag& k, index_t& out_index) const
        {
#pragma HLS inline
            maybe<index_t> index = lookup(h(k), k);

            if (!index.valid() || !valid[index.value()] || tags[index.value()] != k) {
                return maybe<Value>();
            }

            Value value = values[index.value()];
            out_index = index.value();
            return maybe<Value>(value);
        }

        maybe<Value> find(const Tag& k) const
        {
#pragma HLS inline
            index_t index;

            return find(k, index);
        }

        /* For debugging */
#ifdef CACHE_ENABLE_DEBUG_COMMANDS
        bool set_entry(index_t index, bool set_valid, const Tag& tag, const Value& value) {
            bool result = !valid[index];

            valid[index] = set_valid;
            tags[index] = tag;
            values[index] = value;

            return result;
        }

        const Tag& get_tag(index_t index) const { return tags[index % Size]; }
        const Value& get_value(index_t index) const { return values[index % Size]; }
        bool get_valid(index_t index) const { return valid[index % Size]; }
#endif

    private:
        index_t h(const Tag& tag) const { return boost::hash<Tag>()(tag) % Size; }

        maybe<index_t> lookup(index_t hash, const Tag& tag) const {
            for (int i = 0; i < max_hops; ++i) {
                hash = (hash + 1) % Size;
                if (!valid[hash] || tags[hash] == tag)
                    return maybe<index_t>(hash);
            }

            return maybe<index_t>();
        }

        Tag tags[Size];
        Value values[Size];
        bool valid[Size];
    };
}
