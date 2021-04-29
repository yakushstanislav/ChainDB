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

#include "storage.pb.h"

#include "Storage/Block.h"
#include "System/Logger.h"
#include "Crypto/Random.h"

using namespace Core::Storage;
using namespace Core::Crypto;

Block::Container::Container(
    const SHA256::Hash::Ptr hash,
    const SHA256::Hash::Ptr prevHash,
    const Nonce::Ptr nonce,
    const Data& data,
    const Secp256k1::Signature::Ptr signature) :
    _hash(hash),
    _prevHash(prevHash),
    _nonce(nonce),
    _data(data),
    _signature(signature)
{
}

Block::Container::~Container()
{
}

Core::Crypto::SHA256::Hash::Ptr Block::Container::getHash() const
{
    return _hash;
}

Core::Crypto::SHA256::Hash::Ptr Block::Container::getPrevHash() const
{
    return _prevHash;
}

Block::Container::Nonce::Ptr Block::Container::getNonce() const
{
    return _nonce;
}

Block::Container::Data Block::Container::getData() const
{
    return _data;
}

Core::Crypto::Secp256k1::Signature::Ptr Block::Container::getSignature() const
{
    return _signature;
}

bool Block::Container::pack(const Block::Container::Ptr container, Data& outbuf)
{
    Service::Blockchain::Block data;

    data.set_hash(container->getHash()->data(),
        container->getHash()->length());

    data.set_prev_hash(container->getPrevHash()->data(),
        container->getPrevHash()->length());

    data.set_nonce(container->getNonce()->data(),
        container->getNonce()->length());

    data.set_data(container->getData());

    data.set_signature(container->getSignature()->data(),
        container->getSignature()->length());

    return data.SerializeToString(&outbuf);
}

Block::Container::Ptr Block::Container::unpack(const Data& inbuf)
{
    Service::Blockchain::Block data;

    if (!data.ParseFromString(inbuf))
    {
        return nullptr;
    }

    SHA256::Hash::Value hash;

    if (data.hash().size() != sizeof(hash))
    {
        return nullptr;
    }

    std::memcpy(hash, data.hash().data(), sizeof(hash));

    SHA256::Hash::Value prevHash;

    if (data.prev_hash().size() != sizeof(prevHash))
    {
        return nullptr;
    }

    std::memcpy(prevHash, data.prev_hash().data(), sizeof(prevHash));

    Block::Container::Nonce::Value nonce;

    if (data.nonce().size() != sizeof(nonce))
    {
        return nullptr;
    }

    std::memcpy(nonce, data.nonce().data(), sizeof(nonce));

    Secp256k1::Signature::Value signature;

    if (data.signature().size() != sizeof(signature))
    {
        return nullptr;
    }

    std::memcpy(signature, data.signature().data(), sizeof(signature));

    return std::make_shared<Block::Container>(
        std::make_shared<Crypto::SHA256::Hash>(hash),
        std::make_shared<Crypto::SHA256::Hash>(prevHash),
        std::make_shared<Block::Container::Nonce>(nonce),
        data.data(),
        std::make_shared<Secp256k1::Signature>(signature)
    );
}

Block::Block(const Container::Ptr data) :
    _data(data)
{
}

Block::~Block()
{
}

Block::Container::Ptr Block::getData() const
{
    return _data;
}

Block::Container::Nonce::Ptr Block::generateNonce()
{
    Block::Container::Nonce::Value data = {0};

    if (!Random::random(data, sizeof(data)))
    {
        Logger::error("Can\'t generate nonce value");

        return nullptr;
    }

    return std::make_shared<Block::Container::Nonce>(data);
}