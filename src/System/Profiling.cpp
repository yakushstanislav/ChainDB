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

#include "System/Profiling.h"
#include "System/Logger.h"

using namespace Core;

#ifdef _DEBUG_MODE

TrackTimeScope::TrackTimeScope(const std::string& name) :
    _name(name),
    _measureNumber(0)
{
    reset();
}

TrackTimeScope::~TrackTimeScope()
{
    print();
}

void TrackTimeScope::reset()
{
    _startTime = std::chrono::high_resolution_clock::now();

    _measureNumber = 0;
}

void TrackTimeScope::print()
{
    const Clock endTime = std::chrono::high_resolution_clock::now();

    const size_t resultMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _startTime).count();

    Logger::debug("{}({}) scope took {} ms", _name, _measureNumber++, resultMs);
}

#else

TrackTimeScope::TrackTimeScope(const std::string&)
{
}

TrackTimeScope::~TrackTimeScope()
{
}

void TrackTimeScope::reset()
{
}

void TrackTimeScope::print()
{
}

#endif