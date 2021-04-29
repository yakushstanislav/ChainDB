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

#include <map>
#include <string>
#include <variant>

#include "System/IApplication.h"

class ApplicationTest : public Core::System::IApplication
{
public:
    typedef Core::System::IApplication::HandlerMap HandlerMap;

    explicit ApplicationTest(const Core::System::IApplication::HandlerMap& handlers) :
        _handlers(handlers)
    {
    }

    ~ApplicationTest()
    {
    }

    bool initialize(const int argc, const char* argv[]) override
    {
        if (!parseArgs(argc, argv, _handlers))
        {
            return false;
        }

        return true;
    }

    bool run() override
    {
        return true;
    }

private:
    Core::System::IApplication::HandlerMap _handlers;
};

TEST(ApplicationTest, Initialization)
{
    bool boolValue = false;
    int integerValue = 0;
    std::string stringValue;

    const ApplicationTest::HandlerMap handlers = {
        {"--bool", &boolValue},
        {"--integer", &integerValue},
        {"--string", &stringValue}
    };

    const char* argv[] = {
        "<path>",
        "--bool", "true",
        "--integer", "2000",
        "--string", "value"
    };

    ApplicationTest app(handlers);

    EXPECT_TRUE(app.initialize(7, argv));

    EXPECT_EQ(boolValue, true);
    EXPECT_EQ(integerValue, 2000);
    EXPECT_EQ(stringValue, "value");

    EXPECT_TRUE(app.run());
}

TEST(ApplicationTest, ParseBool)
{
    bool boolValue1 = false;
    bool boolValue2 = false;
    bool boolValue3 = false;
    bool boolValue4 = false;

    const ApplicationTest::HandlerMap handlers = {
        {"--bool1", &boolValue1},
        {"--bool2", &boolValue2},
        {"--bool3", &boolValue3},
        {"--bool4", &boolValue4}
    };

    const char* argv[] = {
        "<path>",
        "--bool1", "true",
        "--bool2", "false",
        "--bool3", "0",
        "--bool4", "1",
    };

    ApplicationTest app(handlers);

    EXPECT_TRUE(app.initialize(9, argv));

    EXPECT_EQ(boolValue1, true);
    EXPECT_EQ(boolValue2, false);
    EXPECT_EQ(boolValue3, false);
    EXPECT_EQ(boolValue4, false);

    EXPECT_TRUE(app.run());
}

TEST(ApplicationTest, ParseInteger)
{
    int integerValue1 = 0;
    int integerValue2 = 0;
    int integerValue3 = 0;

    const ApplicationTest::HandlerMap handlers = {
        {"--integer1", &integerValue1},
        {"--integer2", &integerValue2},
        {"--integer3", &integerValue3},
    };

    const char* argv[] = {
        "<path>",
        "--integer1", "2500",
        "--integer2", "1010101010",
        "--integer3", "-7500",
    };

    ApplicationTest app(handlers);

    EXPECT_TRUE(app.initialize(7, argv));

    EXPECT_EQ(integerValue1, 2500);
    EXPECT_EQ(integerValue2, 1010101010);
    EXPECT_EQ(integerValue3, -7500);

    EXPECT_TRUE(app.run());
}

TEST(ApplicationTest, ParseInvalidInteger)
{
    int integerValue1 = 0;

    const ApplicationTest::HandlerMap handlers = {
        {"--number-value", &integerValue1},
    };

    const char* argv[] = {
        "<path>",
        "--number-value", "test",
    };

    ApplicationTest app(handlers);

    EXPECT_FALSE(app.initialize(3, argv));

    EXPECT_EQ(integerValue1, 0);
}

TEST(ApplicationTest, ParseString)
{
    std::string stringValue1;
    std::string stringValue2;

    const ApplicationTest::HandlerMap handlers = {
        {"--string1", &stringValue1},
        {"--string2", &stringValue2}
    };

    const char* argv[] = {
        "<path>",
        "--string1", "test",
        "--string2", "0700/$%@^&-Value"
    };

    ApplicationTest app(handlers);

    EXPECT_TRUE(app.initialize(5, argv));

    EXPECT_EQ(stringValue1, "test");
    EXPECT_EQ(stringValue2, "0700/$%@^&-Value");

    EXPECT_TRUE(app.run());
}