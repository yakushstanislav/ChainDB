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

#include "Storage/Manager.h"

class ManagerTest : public BaseTest
{
};

TEST_F(ManagerTest, CreateChain)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));
    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, CreateChainTwice)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));
    EXPECT_FALSE(manager.createChain(1, "You can\'t steer a parked car"));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, RemoveChain)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));
    EXPECT_TRUE(manager.removeChain(1));
    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, AddBlock)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    EXPECT_FALSE(manager.addBlock(0, "You can\'t steer a parked bike"));
    EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, GetBlock)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));

    EXPECT_FALSE(manager.getBlock(1, 0));
    EXPECT_TRUE(manager.getBlock(1, 1));
    EXPECT_FALSE(manager.getBlock(1, 2));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, GetBlocks)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    for (size_t i = 0; i < 32; i++)
    {
        EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));
    }

    Core::Storage::Manager::BlockList blocks;

    EXPECT_FALSE(manager.getBlocks(0, blocks));
    EXPECT_TRUE(manager.getBlocks(1, blocks));
    EXPECT_FALSE(manager.getBlocks(2, blocks));

    EXPECT_EQ(blocks.size(), 32);

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, VerifyChain)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    for (size_t i = 0; i < 64; i++)
    {
        EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));
    }

    EXPECT_TRUE(manager.verifyChain(1));
    EXPECT_FALSE(manager.verifyChain(2));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, GetChainHeader)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    for (size_t i = 0; i < 8; i++)
    {
        EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));
    }

    const Core::Storage::Chain::Header::Ptr header = manager.getChainHeader(1);

    EXPECT_TRUE(header);

    EXPECT_EQ(header->getVersion(), DB_VERSION);
    EXPECT_EQ(header->getIndex(), 8);

    EXPECT_TRUE(header->getData().size());
    EXPECT_TRUE(header->getPrivateKey());
    EXPECT_TRUE(header->getPublicKey());

    EXPECT_FALSE(manager.getChainHeader(2));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}

TEST_F(ManagerTest, GetChainInfo)
{
    const std::string& path = createTempDirectory();

    Core::Storage::Manager manager(path);

    EXPECT_TRUE(manager.createChain(1, "You can\'t steer a parked car"));

    for (size_t i = 0; i < 8; i++)
    {
        EXPECT_TRUE(manager.addBlock(1, "You can\'t steer a parked bike"));
    }

    size_t version = 0;
    size_t index = 0;

    EXPECT_TRUE(manager.getChainInfo(1, version, index));

    EXPECT_EQ(version, DB_VERSION);
    EXPECT_EQ(index, 8);

    EXPECT_FALSE(manager.getChainInfo(2, version, index));

    EXPECT_TRUE(manager.removeChain(1));

    EXPECT_TRUE(removeDirectory(path));
}