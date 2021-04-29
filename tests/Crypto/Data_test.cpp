/*
   Copyright (c) 2021 Stanislav Yakush (st.yakush@yandex.ru)

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <gtest/gtest.h>

#include "Crypto/Data.h"

TEST(Data, Bytes8)
{
    typedef Core::Crypto::Data<8> Data;

    const Data::Value& value = {1, 2, 3, 4, 5, 6, 7, 8};

    const Data data(value);

    EXPECT_EQ(memcmp(data.data(), value, data.length()), 0);
    EXPECT_EQ(data.length(), 8);
}

TEST(Data, Bytes2048)
{
    typedef Core::Crypto::Data<2048> Data;

    Data::Value value = {0};

    for (size_t i = 0; i < 2048; i++)
    {
        value[i] = i % 255;
    }

    const Data data(value);

    EXPECT_EQ(memcmp(data.data(), value, data.length()), 0);
    EXPECT_EQ(data.length(), 2048);
}

TEST(Data, Integers2048)
{
    typedef Core::Crypto::Data<2048, int> Data;

    Data::Value value = {0};

    for (size_t i = 0; i < 2048; i++)
    {
        value[i] = i;
    }

    const Data data(value);

    EXPECT_EQ(memcmp(data.data(), value, data.length()), 0);
    EXPECT_EQ(data.length(), 8192);
}
