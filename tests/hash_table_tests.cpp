//
// Copyright (c) 2016-2017 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
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

#define CACHE_ENABLE_DEBUG_COMMANDS

#include <ntl/cache.hpp>
#include "gtest/gtest.h"

using std::tuple;
using std::make_tuple;
using std::get;

using ntl::GW_DONE;
using ntl::GW_BUSY;
using ntl::GW_FAIL;

using ntl::maybe;
using ntl::make_maybe;

std::size_t hash_value(const ap_uint<16>& key)
{
    return boost::hash_value(key.to_short());
}

namespace {
     
    typedef ap_uint<16> k; typedef ap_uint<16> v;
    typedef tuple<k, v> value_type;
    typedef maybe<value_type> maybe_value_t;

    class hash_table_wrapper_tests : public ::testing::Test {
    protected:
        // SetUp() is run immediately before a test starts.
        virtual void SetUp() {
        }

        // TearDown() is invoked immediately after a test finishes.
        virtual void TearDown() {
        }

        int add_flow(const value_type& value)
        {
            int ret;
            int result;

            do {
                ret = ht.gateway_add_entry(value, &result);
                progress();
            } while (ret == GW_BUSY);

            EXPECT_EQ(ret, GW_DONE);
            return result;
        }

        int delete_flow(const k& tag)
        {
            int ret;
            int result;

            do {
                ret = ht.gateway_delete_entry(tag, &result);
                progress();
            } while (ret == GW_BUSY);

            EXPECT_EQ(ret, GW_DONE);
            return result;
        }

        void debug_command(uint32_t address, bool write, maybe_value_t& entry)
        {
            int ret;

            do {
                ret = ht.gateway_debug_command(address, write, entry);
                progress();
            } while (ret == GW_BUSY);

            EXPECT_EQ(ret, GW_DONE);
        }

        void progress()
        {
            ht.hash_table();
        }

        maybe_value_t get_entry(uint32_t address)
        {
            maybe_value_t entry;
            debug_command(address, false, entry);
            return entry;
        }

        void set_entry(uint32_t address, const maybe_value_t& entry)
        {
            maybe_value_t e = entry;
            debug_command(address, false, e);
        }

	maybe<tuple<unsigned int, v>> lookup(const k& key)
	{
            ht.lookups.write(key);
            for (int i = 0; i < 15; ++i) {
                progress();

                if (ht.results.empty())
                    continue;

                return ht.results.read();
            }

            assert(false);
	}

        ntl::hash_table_wrapper<k, v, 1024> ht;
    };

    TEST_F(hash_table_wrapper_tests, add_delete)
    {
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 100; ++i) {
                k f = i;
                v value(i);

                auto result = lookup(f);
                EXPECT_FALSE(result.valid()) << "expect invalid entry " << i;

                uint32_t index = add_flow(make_tuple(f, value));
                EXPECT_NE(0, index);
                EXPECT_EQ(0, add_flow(make_tuple(f, value)));

                result = lookup(f);
                EXPECT_TRUE(result.valid()) << "expect valid entry " << i;
                EXPECT_EQ(get<1>(result.value()), value);
                EXPECT_EQ(get<0>(result.value()), index);

                maybe_value_t entry;
                entry = get_entry(index - 1);
                EXPECT_TRUE(entry.valid());
                EXPECT_EQ(make_tuple(f, value), entry.value());

                EXPECT_TRUE(delete_flow(f));
                EXPECT_FALSE(delete_flow(f));

                result = lookup(f);
                EXPECT_FALSE(result.valid()) << "expect invalid entry " << i;

                entry = get_entry(index - 1);
                EXPECT_FALSE(entry.valid());
            }
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
