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

#ifdef _DEBUG_MODE

#include "System/Profiling.h"

class ProfilingTest : public BaseTest
{
};

TEST_F(ProfilingTest, Measure5Milliseconds)
{
    Core::TrackTimeScope scope("test");

    waitMs(5);

    scope.print();

    EXPECT_EQ(getLogData(), "test(0) scope took 5 ms\n");
}

TEST_F(ProfilingTest, MeasureOneSecond)
{
    Core::TrackTimeScope scope("test");

    waitSec(1);

    scope.print();

    EXPECT_EQ(getLogData(), "test(0) scope took 1000 ms\n");
}

TEST_F(ProfilingTest, Measure5MillisecondsTwice)
{
    Core::TrackTimeScope scope("test");

    waitMs(5);

    scope.print();

    EXPECT_EQ(getLogData(), "test(0) scope took 5 ms\n");

    waitMs(5);

    scope.print();

    EXPECT_EQ(getLogData(), "test(1) scope took 10 ms\n");
}

#endif