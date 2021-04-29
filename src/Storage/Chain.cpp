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

#include "storage.pb.h"

#include "Defs.h"
#include "Storage/Chain.h"
#include "Storage/Protocol.h"
#include "System/Logger.h"

using namespace Core::Storage;
using namespace Core::Crypto;

Chain::Header::Header(
    const size_t version,
    const Data& data,
    Secp256k1::PrivateKey::Ptr privateKey,
    Secp256k1::PublicKey::Ptr publicKey) :
    _version(version),
    _index(0),
    _data(data),
    _privateKey(privateKey),
    _publicKey(publicKey)
{
}

Chain::Header::Header(
    const size_t version,
    const size_t index,
    const Data& data,
    Secp256k1::PrivateKey::Ptr privateKey,
    Secp256k1::PublicKey::Ptr publicKey) :
    _version(version),
    _index(index),
    _data(data),
    _privateKey(privateKey),
    _publicKey(publicKey)
{
}

Chain::Header::~Header()
{
}

size_t Chain::Header::getVersion() const
{
    return _version;
}

size_t Chain::Header::getIndex() const
{
    return _index;
}

void Chain::Header::setIndex(size_t index)
{
    _index = index;
}

Chain::Header::Data Chain::Header::getData() const
{
    return _data;
}

Core::Crypto::Secp256k1::PrivateKey::Ptr Chain::Header::getPrivateKey() const
{
    return _privateKey;
}

Core::Crypto::Secp256k1::PublicKey::Ptr Chain::Header::getPublicKey() const
{
    return _publicKey;
}

bool Chain::Header::pack(const Chain::Header::Ptr header, Data& outbuf)
{
    Service::Blockchain::Header data;

    data.set_version(header->getVersion());
    data.set_index(header->getIndex());
    data.set_data(header->getData());

    data.set_private_key(header->getPrivateKey()->data(),
        header->getPrivateKey()->length());

    data.set_public_key(header->getPublicKey()->data(),
        header->getPublicKey()->length());

    return data.SerializeToString(&outbuf);
}

Chain::Header::Ptr Chain::Header::unpack(const Data& inbuf)
{
    Service::Blockchain::Header data;

    if (!data.ParseFromString(inbuf))
    {
        return nullptr;
    }

    Secp256k1::PrivateKey::Value privateKey;

    if (data.private_key().size() != sizeof(privateKey))
    {
        return nullptr;
    }

    std::memcpy(privateKey, data.private_key().data(), sizeof(privateKey));

    Secp256k1::PublicKey::Value publicKey;

    if (data.public_key().size() != sizeof(publicKey))
    {
        return nullptr;
    }

    std::memcpy(publicKey, data.public_key().data(), sizeof(publicKey));

    return std::make_shared<Chain::Header>(data.version(),
        data.index(),
        data.data(),
        std::make_shared<Secp256k1::PrivateKey>(privateKey),
        std::make_shared<Secp256k1::PublicKey>(publicKey));
}

Chain::Chain(const std::string& path) :
    _path(path)
{
}

Chain::~Chain()
{
}

bool Chain::create(const Chain::Header::Data& data,
    Secp256k1::PrivateKey::Ptr privateKey,
    Secp256k1::PublicKey::Ptr publicKey) const
{
    const Chain::Header::Ptr header(new Chain::Header(DB_VERSION, data, privateKey, publicKey));

    Chain::Header::Data buffer;

    if (!Chain::Header::pack(header, buffer))
    {
        Logger::error("Can\'t serialize header");
        return false;
    }

    Storage storage(_path);

    if (!storage.create())
    {
        return false;
    }

    if (!storage.set({{DB_HEADER_KEY, buffer}}))
    {
        return false;
    }

    return true;
}

bool Chain::addBlock(const Block::Ptr block) const
{
    Storage storage(_path);

    if (!storage.open())
    {
        return false;
    }

    const Chain::Header::Ptr header = getHeader(storage);

    if (!header)
    {
        return false;
    }

    header->setIndex(header->getIndex() + 1);

    Chain::Header::Data headerData;

    if (!Chain::Header::pack(header, headerData))
    {
        Logger::error("Can\'t serialize header");
        return false;
    }

    Block::Container::Data blockData;

    if (!Block::Container::pack(block->getData(), blockData))
    {
        Logger::error("Can\'t serialize block");
        return false;
    }

    if (!storage.set({
        {DB_HEADER_KEY, headerData},
        {makeBlockName(header->getIndex()), blockData}}))
    {
        return false;
    }

    return true;
}

Block::Ptr Chain::getBlock(const size_t index) const
{
    Storage storage(_path);

    if (!storage.open())
    {
        return nullptr;
    }

    const Chain::Header::Ptr header = getHeader(storage);

    if (!header)
    {
        return nullptr;
    }

    if (!index || header->getIndex() < index)
    {
        Logger::error("Invalid index {}", index);
        return nullptr;
    }

    const Storage::KeyValue::Ptr data = storage.get(makeBlockName(index));

    if (!data)
    {
        return nullptr;
    }

    Block::Container::Ptr container = Block::Container::unpack(data->getValue());

    if (!container)
    {
        Logger::error("Can\'t parse block (Index: {})", index);
        return nullptr;
    }

    return std::make_shared<Block>(container);
}

bool Chain::getBlocks(std::vector<Block::Ptr>& blocks) const
{
    Storage storage(_path);

    if (!storage.open())
    {
        return false;
    }

    const Chain::Header::Ptr header = getHeader(storage);

    if (!header)
    {
        return false;
    }

    for (size_t i = 1; i <= header->getIndex(); i++)
    {
        const Storage::KeyValue::Ptr value = storage.get(makeBlockName(i));

        if (!value)
        {
            return false;
        }

        Block::Container::Ptr container = Block::Container::unpack(value->getValue());

        if (!container)
        {
            Logger::error("Can\'t parse block (Index: {})", i);
            return false;
        }

        blocks.push_back(std::make_shared<Block>(container));
    }

    return true;
}

bool Chain::remove() const
{
    return Storage(_path).remove();
}

Chain::Header::Ptr Chain::getHeader() const
{
    Storage storage(_path);

    if (!storage.open())
    {
        return nullptr;
    }

    const Storage::KeyValue::Ptr value = storage.get(DB_HEADER_KEY);

    if (!value)
    {
        return nullptr;
    }

    return Chain::Header::unpack(value->getValue());
}

Chain::Header::Ptr Chain::getHeader(const Storage& storage) const
{
    const Storage::KeyValue::Ptr value = storage.get(DB_HEADER_KEY);

    if (!value)
    {
        return nullptr;
    }

    const Chain::Header::Ptr header = Chain::Header::unpack(value->getValue());

    if (!header)
    {
        Logger::error("Can\'t parse header");
        return nullptr;
    }

    if (header->getVersion() != DB_VERSION)
    {
        Logger::error("DB version {} is not supported", header->getVersion());
        return nullptr;
    }

    return header;
}

std::string Chain::makeBlockName(const size_t index) const
{
    return DB_BLOCK_KEY + std::to_string(index);
}