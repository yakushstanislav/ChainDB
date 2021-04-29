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

#include "Crypto/SHA256.h"

TEST(SHA256, GetHashSingleString)
{
    const std::string& data = "You can\'t steer a parked car";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({data});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({data});

    EXPECT_TRUE(hash2);

    EXPECT_EQ(memcmp(hash1->data(), hash2->data(), hash1->length()), 0);
}

TEST(SHA256, GetHashSingleStringInvalid)
{
    const std::string& data1 = "You can\'t steer a parked car";
    const std::string& data2 = "You can\'t steer a parked bike";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({data1});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({data2});

    EXPECT_TRUE(hash2);

    EXPECT_EQ(!memcmp(hash1->data(), hash2->data(), hash1->length()), 0);
}

TEST(SHA256, GetHashMultipleString)
{
    const std::string& data1 = "You can\'t steer a parked car";
    const std::string& data2 = "You can\'t steer a parked bike";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({
        data1,
        data2
    });

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({
        data1,
        data2
    });

    EXPECT_TRUE(hash2);

    EXPECT_EQ(memcmp(hash1->data(), hash2->data(), hash1->length()), 0);
}

TEST(SHA256, GetHashMultipleStringInvalid)
{
    const std::string& data1 = "You can\'t steer a parked car";
    const std::string& data2 = "You can\'t steer a parked bike";
    const std::string& data3 = "You can\'t steer a parked helicopter";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({
        data1,
        data2
    });

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({
        data1,
        data3
    });

    EXPECT_TRUE(hash2);

    EXPECT_EQ(!memcmp(hash1->data(), hash2->data(), hash1->length()), 0);
}

TEST(SHA256, GetHashTwice)
{
    const std::string& data = "You can\'t steer a parked car";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({data});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({{
        reinterpret_cast<const char*>(hash1->data()), hash1->length()}
    });

    EXPECT_TRUE(hash2);

    const Core::Crypto::SHA256::Hash::Ptr hash3 = Core::Crypto::SHA256::getHashN({data});

    EXPECT_TRUE(hash3);

    EXPECT_EQ(memcmp(hash2->data(), hash3->data(), hash2->length()), 0);
}