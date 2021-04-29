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

#include <memory>

#include "Defs.h"
#include "Crypto/Data.h"
#include "Crypto/ECDSA.h"
#include <Crypto/SHA256.h>

namespace Core::Storage
{

class Block
{
public:
    typedef std::shared_ptr<Block> Ptr;

    struct Container
    {
        public:
            typedef std::shared_ptr<Container> Ptr;
            typedef Crypto::Data<NONCE_LENGTH> Nonce;
            typedef std::string Data;

            Container(const Crypto::SHA256::Hash::Ptr hash,
                const Crypto::SHA256::Hash::Ptr prevHash,
                const Nonce::Ptr nonce,
                const Data& data,
                const Crypto::Secp256k1::Signature::Ptr signature);
            ~Container();

            Crypto::SHA256::Hash::Ptr getHash() const;
            Crypto::SHA256::Hash::Ptr getPrevHash() const;

            Nonce::Ptr getNonce() const;
            Data getData() const;

            Crypto::Secp256k1::Signature::Ptr getSignature() const;

            static bool pack(const Block::Container::Ptr container, Data& outbuf);
            static Block::Container::Ptr unpack(const Data& inbuf);

        private:
            Crypto::SHA256::Hash::Ptr _hash;
            Crypto::SHA256::Hash::Ptr _prevHash;
            Nonce::Ptr _nonce;
            Data _data;
            Crypto::Secp256k1::Signature::Ptr _signature;
    };

    explicit Block(const Container::Ptr data);
    ~Block();

    Container::Ptr getData() const;

    static Container::Nonce::Ptr generateNonce();

private:
    Container::Ptr _data;
};

}