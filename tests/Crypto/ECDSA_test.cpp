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

#include "Crypto/ECDSA.h"
#include "Crypto/SHA256.h"

TEST(ECDSA, GeneratePrivateKey)
{
    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);
}

TEST(ECDSA, CreatePublicKey)
{
    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);
}

TEST(ECDSA, GetSignature)
{
    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Crypto::SHA256::Hash::Ptr hash = Core::Crypto::SHA256::getHash({"You can\'t steer a parked car"});

    EXPECT_TRUE(hash);

    const Core::Crypto::Secp256k1::Signature::Ptr signature = secp256k1.getSignature(hash, privateKey);

    EXPECT_TRUE(signature);
}

TEST(ECDSA, VerifySignature)
{
    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Crypto::SHA256::Hash::Ptr hash = Core::Crypto::SHA256::getHash({"You can\'t steer a parked car"});

    EXPECT_TRUE(hash);

    const Core::Crypto::Secp256k1::Signature::Ptr signature = secp256k1.getSignature(hash, privateKey);

    EXPECT_TRUE(signature);

    EXPECT_TRUE(secp256k1.verifySignature(hash, publicKey, signature));
}

TEST(ECDSA, VerifySignatureInvalid)
{
    const Core::Crypto::Secp256k1 secp256k1;

    const Core::Crypto::Secp256k1::PrivateKey::Ptr privateKey = secp256k1.generatePrivateKey();

    EXPECT_TRUE(privateKey);

    const Core::Crypto::Secp256k1::PublicKey::Ptr publicKey = secp256k1.createPublicKey(privateKey);

    EXPECT_TRUE(publicKey);

    const Core::Crypto::SHA256::Hash::Ptr hash = Core::Crypto::SHA256::getHash({"You can\'t steer a parked car"});

    EXPECT_TRUE(hash);

    const Core::Crypto::SHA256::Hash::Ptr hash2 = Core::Crypto::SHA256::getHash({"You can\'t steer a parked bike"});

    EXPECT_TRUE(hash2);

    const Core::Crypto::Secp256k1::Signature::Ptr signature = secp256k1.getSignature(hash, privateKey);

    EXPECT_TRUE(signature);

    EXPECT_FALSE(secp256k1.verifySignature(hash2, publicKey, signature));
}