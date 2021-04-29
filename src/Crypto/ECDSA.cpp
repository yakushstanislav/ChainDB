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

#include "Crypto/ECDSA.h"
#include "Crypto/Random.h"

using namespace Core::Crypto;

Secp256k1::Secp256k1()
{
    _ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
}

Secp256k1::~Secp256k1()
{
    secp256k1_context_destroy(_ctx);
}

Secp256k1::PrivateKey::Ptr Secp256k1::generatePrivateKey() const
{
    PrivateKey::Value data;

    if (!Random::random(data, sizeof(data), true))
    {
        return nullptr;
    }

    if (!secp256k1_ec_seckey_verify(_ctx, data))
    {
        return nullptr;
    }

    return std::make_shared<PrivateKey>(data);
}

Secp256k1::PublicKey::Ptr Secp256k1::createPublicKey(const PrivateKey::Ptr privateKey) const
{
    secp256k1_pubkey publicKey;

    if (!secp256k1_ec_pubkey_create(_ctx, &publicKey, privateKey->data()))
    {
        return nullptr;
    }

    PublicKey::Value data;
    size_t keysize = sizeof(data);

    if (!secp256k1_ec_pubkey_serialize(_ctx, data, &keysize, &publicKey, SECP256K1_EC_COMPRESSED))
    {
        return nullptr;
    }

    return std::make_shared<PublicKey>(data);
}

Secp256k1::Signature::Ptr Secp256k1::getSignature(const SHA256::Hash::Ptr hash, const PrivateKey::Ptr privateKey) const
{
    secp256k1_ecdsa_signature signature;

    if (!secp256k1_ecdsa_sign(_ctx, &signature, hash->data(), privateKey->data(),
        secp256k1_nonce_function_rfc6979, nullptr))
    {
        return nullptr;
    }

    Signature::Value data;

    if (!secp256k1_ecdsa_signature_serialize_compact(_ctx, data, &signature))
    {
        return nullptr;
    }

    return std::make_shared<Signature>(data);
}

bool Secp256k1::verifySignature(const SHA256::Hash::Ptr hash, const PublicKey::Ptr publicKey, const Signature::Ptr signature) const
{
    secp256k1_pubkey pubKey;

    if (!secp256k1_ec_pubkey_parse(_ctx, &pubKey, publicKey->data(), publicKey->length()))
    {
        return false;
    }

    secp256k1_ecdsa_signature sign;

    if (!secp256k1_ecdsa_signature_parse_compact(_ctx, &sign, signature->data()))
    {
        return false;
    }

    return secp256k1_ecdsa_verify(_ctx, &sign, hash->data(), &pubKey);
}