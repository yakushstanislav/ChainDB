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

#include <cstring>

#include "Crypto/SHA256.h"

using namespace Core::Crypto;

SHA256::Hash::Ptr SHA256::getHash(const std::vector<std::string>& input)
{
    SHA256_CTX ctx;
    SHA256::Hash::Value output;

    if (!SHA256_Init(&ctx))
    {
        return nullptr;
    }

    for (const std::string& data : input)
    {
        if (!SHA256_Update(&ctx, data.data(), data.size()))
        {
            return nullptr;
        }
    }

    if (!SHA256_Final(output, &ctx))
    {
        return nullptr;
    }

    return std::make_shared<SHA256::Hash>(output);
}

SHA256::Hash::Ptr SHA256::getHashN(const std::vector<std::string>& input, const size_t n)
{
    SHA256::Hash::Ptr hash = getHash(input);

    if (!hash)
    {
        return nullptr;
    }

    for (size_t i = 0; i < n - 1; i++)
    {
        hash = getHash({{reinterpret_cast<const char*>(hash->data()), hash->length()}});

        if (!hash)
        {
            return nullptr;
        }
    }

    return hash;
}