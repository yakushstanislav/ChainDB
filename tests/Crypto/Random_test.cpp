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

#include "Crypto/Random.h"

#ifdef _TEST_PRINT
static void printHex(const uint8_t* data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%.02X ", data[i]);
    }

    printf("\n");
}
#else
static void printHex(const uint8_t*, size_t)
{
}
#endif

TEST(Random, Status)
{
    EXPECT_TRUE(Core::Crypto::Random::status());
}

TEST(Random, Poll)
{
    EXPECT_TRUE(Core::Crypto::Random::poll());
}

TEST(Random, Random8Bytes)
{
    uint8_t data[8] = {0};

    EXPECT_TRUE(Core::Crypto::Random::random(data, sizeof(data)));

    printHex(data, sizeof(data));
}

TEST(Random, Random32Bytes)
{
    uint8_t data[32] = {0};

    EXPECT_TRUE(Core::Crypto::Random::random(data, sizeof(data)));

    printHex(data, sizeof(data));
}

TEST(Random, Random128Bytes)
{
    uint8_t data[128] = {0};

    EXPECT_TRUE(Core::Crypto::Random::random(data, sizeof(data)));

    printHex(data, sizeof(data));
}

TEST(Random, Random512Bytes)
{
    uint8_t data[512] = {0};

    EXPECT_TRUE(Core::Crypto::Random::random(data, sizeof(data)));

    printHex(data, sizeof(data));
}