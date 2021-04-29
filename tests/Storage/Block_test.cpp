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

#include "Storage/Block.h"
#include "Crypto/SHA256.h"
#include "Crypto/ECDSA.h"

TEST(Block, Initialization)
{
    const Core::Storage::Block::Container::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash2);

    const Core::Storage::Block::Container::Nonce::Ptr nonce = Core::Storage::Block::generateNonce();

    EXPECT_TRUE(nonce);

    const Core::Storage::Block::Container::Ptr container = std::make_shared<Core::Storage::Block::Container>(
        hash1,
        hash2,
        nonce,
        data,
        nullptr);

    EXPECT_TRUE(container);

    Core::Storage::Block::Ptr block(new Core::Storage::Block(container));

    EXPECT_TRUE(block);

    EXPECT_EQ(block->getData()->getHash(), hash1);
    EXPECT_EQ(block->getData()->getPrevHash(), hash2);
    EXPECT_EQ(block->getData()->getNonce(), nonce);
    EXPECT_EQ(block->getData()->getData(), data);
    EXPECT_EQ(block->getData()->getSignature(), nullptr);
}

TEST(Block, GenerateNonce)
{
    Core::Storage::Block::Container::Nonce::Ptr nonce = Core::Storage::Block::generateNonce();

    EXPECT_TRUE(nonce);
}

TEST(Block, Pack)
{
    const Core::Storage::Block::Container::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash2);

    const Core::Storage::Block::Container::Nonce::Ptr nonce = Core::Storage::Block::generateNonce();

    EXPECT_TRUE(nonce);

    Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::Signature::Ptr signature = secp256k1.getSignature(hash2, privateKey);

    EXPECT_TRUE(signature);

    const Core::Storage::Block::Container::Ptr container = std::make_shared<Core::Storage::Block::Container>(
        hash1,
        hash2,
        nonce,
        data,
        signature);

    EXPECT_TRUE(container);

    EXPECT_EQ(container->getHash(), hash1);
    EXPECT_EQ(container->getPrevHash(), hash2);
    EXPECT_EQ(container->getNonce(), nonce);
    EXPECT_EQ(container->getData(), data);
    EXPECT_EQ(container->getSignature(), signature);

    Core::Storage::Block::Container::Data buffer;

    EXPECT_TRUE(Core::Storage::Block::Container::pack(container, buffer));

    EXPECT_TRUE(buffer.length());
}

TEST(Block, Unpack)
{
    const Core::Storage::Block::Container::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::SHA256::Hash::Ptr hash1 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash1);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({"Hash 1"});

    EXPECT_TRUE(hash2);

    const Core::Storage::Block::Container::Nonce::Ptr nonce = Core::Storage::Block::generateNonce();

    EXPECT_TRUE(nonce);

    Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::Signature::Ptr signature = secp256k1.getSignature(hash2, privateKey);

    EXPECT_TRUE(signature);

    const Core::Storage::Block::Container::Ptr container1 = std::make_shared<Core::Storage::Block::Container>(
        hash1,
        hash2,
        nonce,
        data,
        signature);

    EXPECT_TRUE(container1);

    EXPECT_EQ(container1->getHash(), hash1);
    EXPECT_EQ(container1->getPrevHash(), hash2);
    EXPECT_EQ(container1->getNonce(), nonce);
    EXPECT_EQ(container1->getData(), data);
    EXPECT_EQ(container1->getSignature(), signature);

    Core::Storage::Block::Container::Data buffer;

    EXPECT_TRUE(Core::Storage::Block::Container::pack(container1, buffer));

    EXPECT_TRUE(buffer.length());

    const Core::Storage::Block::Container::Ptr container2 = Core::Storage::Block::Container::unpack(buffer);

    EXPECT_TRUE(container2);

    EXPECT_EQ(memcmp(container2->getHash()->data(), hash1->data(), container2->getHash()->length()), 0);
    EXPECT_EQ(memcmp(container2->getPrevHash()->data(), hash2->data(), container2->getPrevHash()->length()), 0);
    EXPECT_EQ(memcmp(container2->getNonce()->data(), nonce->data(), container2->getNonce()->length()), 0);

    EXPECT_EQ(container2->getData(), data.data());

    EXPECT_EQ(memcmp(container2->getSignature()->data(), signature->data(), container2->getSignature()->length()), 0);
}