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

#include <cstdint>
#include <string>
#include <memory>

#include "Crypto/ECDSA.h"
#include "Storage/Storage.h"
#include "Storage/Block.h"

namespace Core::Storage
{

class Chain
{
public:
    typedef std::shared_ptr<Chain> Ptr;

    struct Header
    {
        public:
            typedef std::shared_ptr<Header> Ptr;
            typedef std::string Data;

            Header(const size_t version,
                const Data& data,
                Crypto::Secp256k1::PrivateKey::Ptr privateKey,
                Crypto::Secp256k1::PublicKey::Ptr publicKey);

            Header(const size_t version,
                const size_t index,
                const Data& data,
                Crypto::Secp256k1::PrivateKey::Ptr privateKey,
                Crypto::Secp256k1::PublicKey::Ptr publicKey);

            ~Header();

            size_t getVersion() const;

            size_t getIndex() const;

            void setIndex(size_t index);

            Data getData() const;

            Crypto::Secp256k1::PrivateKey::Ptr getPrivateKey() const;
            Crypto::Secp256k1::PublicKey::Ptr getPublicKey() const;

            static bool pack(const Chain::Header::Ptr header, Data& outbuf);
            static Chain::Header::Ptr unpack(const Data& inbuf);

        private:
            size_t _version;
            size_t _index;
            Data _data;
            Crypto::Secp256k1::PrivateKey::Ptr _privateKey;
            Crypto::Secp256k1::PublicKey::Ptr _publicKey;
    };

    explicit Chain(const std::string& path);
    ~Chain();

    bool create(const Chain::Header::Data& data,
        Crypto::Secp256k1::PrivateKey::Ptr privateKey,
        Crypto::Secp256k1::PublicKey::Ptr publicKey) const;

    bool addBlock(const Block::Ptr block) const;

    Block::Ptr getBlock(const size_t index) const;

    bool getBlocks(std::vector<Block::Ptr>& blocks) const;

    bool remove() const;

    Header::Ptr getHeader() const;

private:
    Chain::Header::Ptr getHeader(const Storage& storage) const;

    std::string makeBlockName(const size_t index) const;

private:
    std::string _path;
};

}