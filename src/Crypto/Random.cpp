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

#include <openssl/rand.h>

#include "Crypto/Random.h"

using namespace Core::Crypto;

bool Random::status()
{
    return RAND_status();
}

bool Random::poll()
{
    return RAND_poll();
}

bool Random::random(uint8_t* output, const size_t length, const bool priv)
{
    if (!priv)
    {
        if (RAND_bytes(output, length) != 1)
            return false;
    }
    else
    {
        if (RAND_priv_bytes(output, length) != 1)
            return false;
    }

    return true;
}