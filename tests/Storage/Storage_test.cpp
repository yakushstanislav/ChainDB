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

#include "Storage/Storage.h"

class StorageTest : public BaseTest
{
};

TEST_F(StorageTest, Create)
{
    const std::string& path = makeTempPath();

    {
        Core::Storage::Storage storage(path);

        EXPECT_TRUE(storage.create());
    }

    EXPECT_TRUE(Core::Storage::Storage(path).remove());
}

TEST_F(StorageTest, Open)
{
    const std::string& path = makeTempPath();

    {
        Core::Storage::Storage storage(path);

        EXPECT_TRUE(storage.create());
    }

    Core::Storage::Storage storage(path);

    EXPECT_TRUE(storage.open());
    EXPECT_TRUE(storage.close());
    EXPECT_TRUE(storage.remove());
}

TEST_F(StorageTest, OpenTwice)
{
    const std::string& path = makeTempPath();

    {
        Core::Storage::Storage storage(path);

        EXPECT_TRUE(storage.create());
    }

    Core::Storage::Storage storage(path);

    EXPECT_TRUE(storage.open());
    EXPECT_FALSE(storage.open());

    EXPECT_TRUE(storage.close());
    EXPECT_TRUE(storage.remove());
}

TEST_F(StorageTest, CloseTwice)
{
    Core::Storage::Storage storage(makeTempPath());

    EXPECT_TRUE(storage.create());

    EXPECT_TRUE(storage.close());
    EXPECT_FALSE(storage.close());

    EXPECT_TRUE(storage.remove());
}

TEST_F(StorageTest, Set)
{
    Core::Storage::Storage storage(makeTempPath());

    EXPECT_TRUE(storage.create());

    EXPECT_TRUE(storage.set({}));

    EXPECT_TRUE(storage.set({{"", ""}}));
    EXPECT_TRUE(storage.set({{"", ""}}));

    EXPECT_TRUE(storage.set({
        {"Key 1", "Value 1"}
    }));

    EXPECT_TRUE(storage.set({
        {"Key 2", "Value 2"},
        {"Key 3", "Value 3"}
    }));

    EXPECT_TRUE(storage.close());
    EXPECT_TRUE(storage.remove());
}

TEST_F(StorageTest, Get)
{
    Core::Storage::Storage storage(makeTempPath());

    EXPECT_TRUE(storage.create());

    EXPECT_TRUE(storage.set({
        {"Key 1", "Value 1"},
        {"Key 2", "Value 2"}
    }));

    const Core::Storage::Storage::KeyValue::Ptr pair1 = storage.get("Key 1");

    EXPECT_TRUE(pair1);

    EXPECT_EQ(pair1->getKey(), "Key 1");
    EXPECT_EQ(pair1->getValue(), "Value 1");

    const Core::Storage::Storage::KeyValue::Ptr pair2 = storage.get("Key 2");

    EXPECT_TRUE(pair2);

    EXPECT_EQ(pair2->getKey(), "Key 2");
    EXPECT_EQ(pair2->getValue(), "Value 2");

    const Core::Storage::Storage::KeyValue::Ptr pair3 = storage.get("Key not exists");

    EXPECT_FALSE(pair3);

    EXPECT_TRUE(storage.close());
    EXPECT_TRUE(storage.remove());
}

TEST_F(StorageTest, Update)
{
    Core::Storage::Storage storage(makeTempPath());

    EXPECT_TRUE(storage.create());

    EXPECT_TRUE(storage.set({
        {"Key 1", "Value 1"},
        {"Key 2", "Value 2"}
    }));

    const Core::Storage::Storage::KeyValue::Ptr pair1 = storage.get("Key 1");

    EXPECT_TRUE(pair1);

    EXPECT_EQ(pair1->getKey(), "Key 1");
    EXPECT_EQ(pair1->getValue(), "Value 1");

    const Core::Storage::Storage::KeyValue::Ptr pair2 = storage.get("Key 2");

    EXPECT_TRUE(pair2);

    EXPECT_EQ(pair2->getKey(), "Key 2");
    EXPECT_EQ(pair2->getValue(), "Value 2");

    EXPECT_TRUE(storage.set({
        {"Key 1", "Value 3"},
        {"Key 2", "Value 4"}
    }));

    const Core::Storage::Storage::KeyValue::Ptr pair3 = storage.get("Key 1");

    EXPECT_TRUE(pair3);

    EXPECT_EQ(pair3->getKey(), "Key 1");
    EXPECT_EQ(pair3->getValue(), "Value 3");

    const Core::Storage::Storage::KeyValue::Ptr pair4 = storage.get("Key 2");

    EXPECT_TRUE(pair4);

    EXPECT_EQ(pair4->getKey(), "Key 2");
    EXPECT_EQ(pair4->getValue(), "Value 4");

    EXPECT_TRUE(storage.close());
    EXPECT_TRUE(storage.remove());
}