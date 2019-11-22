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

#include <ntl/counter.hpp>
#include <ntl/stream.hpp>
#include <ntl/axi_data.hpp>

using ntl::axi_data;

#include "gtest/gtest.h"

class counter_tests : public ::testing::Test
{
public:
    counter_tests() {}

protected:
    ntl::stream<axi_data> in1, in2;
    ntl::counter<axi_data> regular;
    ntl::counter<axi_data, ntl::maxed_int<2, 2> > stuck;
};

TEST_F(counter_tests, check_stuck)
{
    const int num = 7;

    for (int i = 0; i < num; ++i) {
        in1.write(axi_data(i, 0, 0));
        in2.write(axi_data(i, 0, 0));
        regular.step(in1);
        stuck.step(in2);
    }

    for (int i = 0; i < num; ++i) {
        ASSERT_FALSE(regular.out.empty()) << i;
        ASSERT_FALSE(stuck.out.empty()) << i;
        ASSERT_EQ(regular.out.read(), i) << i;
        ASSERT_EQ(stuck.out.read(), i < 3 ? i : 2) << i;
    }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
