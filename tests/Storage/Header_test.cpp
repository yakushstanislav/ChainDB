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

#include "Storage/Chain.h"
#include "Crypto/ECDSA.h"

TEST(Header, Initialization)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain::Header::Ptr header = std::make_shared<Core::Storage::Chain::Header>(
        DB_VERSION,
        0,
        data,
        privateKey,
        publicKey
    );

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getVersion(), DB_VERSION);
    EXPECT_EQ(header->getIndex(), 0);
    EXPECT_EQ(header->getData(), data);
    EXPECT_EQ(header->getPrivateKey(), privateKey);
    EXPECT_EQ(header->getPublicKey(), publicKey);
}

TEST(Header, Pack)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain::Header::Ptr header = std::make_shared<Core::Storage::Chain::Header>(
        DB_VERSION,
        0,
        data,
        privateKey,
        publicKey
    );

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getVersion(), DB_VERSION);
    EXPECT_EQ(header->getIndex(), 0);
    EXPECT_EQ(header->getData(), data);
    EXPECT_EQ(header->getPrivateKey(), privateKey);
    EXPECT_EQ(header->getPublicKey(), publicKey);

    Core::Storage::Chain::Header::Data buffer;

    EXPECT_TRUE(Core::Storage::Chain::Header::pack(header, buffer));

    EXPECT_TRUE(buffer.length());
}

TEST(Header, Unpack)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain::Header::Ptr header1 = std::make_shared<Core::Storage::Chain::Header>(
        DB_VERSION,
        0,
        data,
        privateKey,
        publicKey
    );

    EXPECT_TRUE(header1);

    EXPECT_EQ(header1->getVersion(), DB_VERSION);
    EXPECT_EQ(header1->getIndex(), 0);
    EXPECT_EQ(header1->getData(), data);
    EXPECT_EQ(header1->getPrivateKey(), privateKey);
    EXPECT_EQ(header1->getPublicKey(), publicKey);

    Core::Storage::Chain::Header::Data buffer;

    EXPECT_TRUE(Core::Storage::Chain::Header::pack(header1, buffer));

    EXPECT_TRUE(buffer.length());

    const Core::Storage::Chain::Header::Ptr header2 = Core::Storage::Chain::Header::unpack(buffer);

    EXPECT_TRUE(header2);

    EXPECT_EQ(header2->getVersion(), DB_VERSION);
    EXPECT_EQ(header2->getIndex(), 0);
    EXPECT_EQ(header2->getData(), data);

    EXPECT_EQ(memcmp(header2->getPrivateKey()->data(), privateKey->data(), header2->getPrivateKey()->length()), 0);
    EXPECT_EQ(memcmp(header2->getPublicKey()->data(), publicKey->data(), header2->getPublicKey()->length()), 0);
}
