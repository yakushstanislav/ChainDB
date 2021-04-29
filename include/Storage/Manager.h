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

#pragma once

#include <string>
#include <vector>

#include "Crypto/ECDSA.h"
#include "Storage/Chain.h"
#include "Storage/Block.h"

namespace Core::Storage
{

class Manager
{
public:
    typedef std::vector<Block::Ptr> BlockList;

    Manager(const std::string& storageDir);
    ~Manager();

    Chain::Ptr createChain(const size_t chainId, const std::string& data) const;

    Block::Ptr addBlock(const size_t chainId, const std::string& data) const;

    Block::Ptr getBlock(const size_t chainId, const size_t index) const;

    bool getBlocks(const size_t chainId, BlockList& blocks) const;

    bool removeChain(const size_t chainId) const;

    bool verifyChain(const size_t chainId) const;

    Chain::Header::Ptr getChainHeader(const size_t chainId) const;

    bool getChainInfo(const size_t chainId, size_t& version, size_t& index) const;

private:
    std::string makeStoragePath(const size_t chainId) const;

private:
    Crypto::Secp256k1 _secp256k1;
    std::string _storageDir;
};

}