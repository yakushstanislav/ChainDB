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

#include "secp256k1.h"

#include "Crypto/Data.h"
#include "Crypto/SHA256.h"

namespace Core::Crypto
{

class Secp256k1
{
public:
    typedef Data<32> PrivateKey;
    typedef Data<33> PublicKey;
    typedef Data<64> Signature;

    Secp256k1();
    ~Secp256k1();

    PrivateKey::Ptr generatePrivateKey() const;

    PublicKey::Ptr createPublicKey(const PrivateKey::Ptr privateKey) const;

    Signature::Ptr getSignature(const SHA256::Hash::Ptr hash, const PrivateKey::Ptr privateKey) const;

    bool verifySignature(const SHA256::Hash::Ptr hash, const PublicKey::Ptr publicKey, const Signature::Ptr signature) const;

private:
    secp256k1_context* _ctx;
};

}