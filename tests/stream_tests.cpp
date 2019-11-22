//
// Copyright (c) 2019 Haggai Eran
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

#include <ntl/peek_stream.hpp>
#include <ntl/stream.hpp>

#include "gtest/gtest.h"

class peek_stream_tests : public ::testing::Test
{
public:
    peek_stream_tests() {}

protected:
    ntl::stream<int> a;
    ntl::peek_stream<int> peek;
};

TEST_F(peek_stream_tests, traverse)
{
    ASSERT_TRUE(a.empty());
    ASSERT_TRUE(peek.empty());

    const int num = 100;

    peek.link(a);

    ASSERT_TRUE(a.empty());
    ASSERT_TRUE(peek.empty());

    for (int i = 0; i < num; ++i) {
        a.write(i);
        ASSERT_FALSE(a.empty());
        peek.link(a);
        ASSERT_FALSE(peek.empty());
    }

    for (int i = 0; i < num; ++i) {
        peek.link(a);
        ASSERT_FALSE(peek.empty());
        ASSERT_EQ(peek.peek(), i);
        ASSERT_EQ(peek.read(), i);
    }

    ASSERT_TRUE(peek.empty());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
