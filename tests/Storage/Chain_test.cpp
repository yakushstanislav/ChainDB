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

#include "BaseTest.h"

#include "Storage/Chain.h"
#include "Storage/Block.h"
#include "Crypto/ECDSA.h"

class ChainTest : public BaseTest
{
public:
    Core::Storage::Block::Ptr getBlock() const
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

        return std::make_shared<Core::Storage::Block>(container);
    }
};

TEST_F(ChainTest, Create)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, CreateTwice)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));
    EXPECT_FALSE(chain.create(data, privateKey, publicKey));

    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, AddBlock)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    const Core::Storage::Block::Ptr block = getBlock();

    EXPECT_TRUE(block);

    EXPECT_TRUE(chain.addBlock(block));

    const Core::Storage::Chain::Header::Ptr header = chain.getHeader();

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getIndex(), 1);

    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, GetBlock)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    const Core::Storage::Block::Ptr firstBlock = getBlock();

    EXPECT_TRUE(firstBlock);

    EXPECT_TRUE(chain.addBlock(firstBlock));

    for (size_t i = 0; i < 7; i++)
    {
        const Core::Storage::Block::Ptr block = getBlock();

        EXPECT_TRUE(block);

        EXPECT_TRUE(chain.addBlock(block));
    }

    EXPECT_FALSE(chain.getBlock(0));

    const Core::Storage::Block::Ptr block = chain.getBlock(1);

    EXPECT_TRUE(block);

    EXPECT_EQ(memcmp(block->getData()->getHash()->data(),
        firstBlock->getData()->getHash()->data(),
        block->getData()->getHash()->length()), 0);

    EXPECT_EQ(memcmp(block->getData()->getPrevHash()->data(),
        firstBlock->getData()->getPrevHash()->data(),
        block->getData()->getPrevHash()->length()), 0);

    EXPECT_EQ(memcmp(block->getData()->getNonce()->data(),
        firstBlock->getData()->getNonce()->data(),
        block->getData()->getNonce()->length()), 0);

    EXPECT_EQ(block->getData()->getData(), firstBlock->getData()->getData());

    EXPECT_EQ(memcmp(block->getData()->getSignature()->data(),
        firstBlock->getData()->getSignature()->data(),
        block->getData()->getSignature()->length()), 0);

    const Core::Storage::Chain::Header::Ptr header = chain.getHeader();

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getIndex(), 8);

    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, GetBlocks)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    std::vector<Core::Storage::Block::Ptr> addedBlocks;

    for (size_t i = 0; i < 10; i++)
    {
        const Core::Storage::Block::Ptr block = getBlock();

        EXPECT_TRUE(block);

        EXPECT_TRUE(chain.addBlock(block));

        addedBlocks.push_back(block);
    }

    std::vector<Core::Storage::Block::Ptr> blocks;

    EXPECT_TRUE(chain.getBlocks(blocks));

    EXPECT_EQ(addedBlocks.size(), blocks.size());

    for (size_t i = 0; i < blocks.size(); i++)
    {
        EXPECT_EQ(memcmp(blocks[i]->getData()->getHash()->data(),
            addedBlocks[i]->getData()->getHash()->data(),
            blocks[i]->getData()->getHash()->length()), 0);

        EXPECT_EQ(memcmp(blocks[i]->getData()->getPrevHash()->data(),
            addedBlocks[i]->getData()->getPrevHash()->data(),
            blocks[i]->getData()->getPrevHash()->length()), 0);

        EXPECT_EQ(memcmp(blocks[i]->getData()->getNonce()->data(),
            addedBlocks[i]->getData()->getNonce()->data(),
            blocks[i]->getData()->getNonce()->length()), 0);

        EXPECT_EQ(blocks[i]->getData()->getData(), addedBlocks[i]->getData()->getData());

        EXPECT_EQ(memcmp(blocks[i]->getData()->getSignature()->data(),
            addedBlocks[i]->getData()->getSignature()->data(),
            blocks[i]->getData()->getSignature()->length()), 0);
    }

    const Core::Storage::Chain::Header::Ptr header = chain.getHeader();

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getIndex(), 10);

    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, Remove)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    EXPECT_TRUE(chain.remove());
    EXPECT_TRUE(chain.remove());
}

TEST_F(ChainTest, GetHeader)
{
    const Core::Storage::Chain::Header::Data& data = "You can\'t steer a parked car";

    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Storage::Chain chain(makeTempPath());

    EXPECT_TRUE(chain.create(data, privateKey, publicKey));

    const Core::Storage::Block::Ptr block = getBlock();

    EXPECT_TRUE(block);

    EXPECT_TRUE(chain.addBlock(block));

    const Core::Storage::Chain::Header::Ptr header = chain.getHeader();

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getVersion(), DB_VERSION);
    EXPECT_EQ(header->getIndex(), 1);

    EXPECT_EQ(memcmp(header->getPrivateKey()->data(), privateKey->data(), header->getPrivateKey()->length()), 0);
    EXPECT_EQ(memcmp(header->getPublicKey()->data(), publicKey->data(), header->getPublicKey()->length()), 0);

    EXPECT_TRUE(chain.remove());
}