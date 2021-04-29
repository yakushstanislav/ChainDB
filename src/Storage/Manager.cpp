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

#include <filesystem>

#include "System/Logger.h"
#include "Storage/Manager.h"

using namespace Core::Storage;
using namespace Core::Crypto;

Manager::Manager(const std::string& storageDir) :
    _storageDir(storageDir)
{
}

Manager::~Manager()
{
}

Chain::Ptr Manager::createChain(const size_t chainId, const std::string& data) const
{
    const Crypto::Secp256k1::PrivateKey::Ptr privateKey = _secp256k1.generatePrivateKey();

    if (!privateKey)
    {
        Logger::error("Can\'t generate private key");
        return nullptr;
    }

    const Crypto::Secp256k1::PublicKey::Ptr publicKey = _secp256k1.createPublicKey(privateKey);

    if (!publicKey)
    {
        Logger::error("Can\'t create public key");
        return nullptr;
    }

    const Chain::Ptr chain(new Chain(makeStoragePath(chainId)));

    if (!chain->create(data, privateKey, publicKey))
    {
        return nullptr;
    }

    return chain;
}

Block::Ptr Manager::addBlock(const size_t chainId, const std::string& data) const
{
    const Chain chain(makeStoragePath(chainId));

    const Chain::Header::Ptr header = chain.getHeader();

    if (!header)
    {
        Logger::error("Can\'t get header");
        return nullptr;
    }

    SHA256::Hash::Ptr prevHash;

    if (!header->getIndex())
    {
        prevHash = SHA256::getHashN({
            header->getData(),
            {reinterpret_cast<const char*>(header->getPrivateKey()->data()), header->getPrivateKey()->length()},
            {reinterpret_cast<const char*>(header->getPublicKey()->data()), header->getPublicKey()->length()}
        });

        if (!prevHash)
        {
            Logger::error("Can\'t calculate header hash");
            return nullptr;
        }
    }
    else
    {
        const Block::Ptr lastBlock = chain.getBlock(header->getIndex());

        if (!lastBlock)
        {
            Logger::error("Can\'t get last block");
            return nullptr;
        }

        prevHash = lastBlock->getData()->getHash();
    }

    const Block::Container::Nonce::Ptr nonce = Block::generateNonce();

    if (!nonce)
    {
        Logger::error("Can\'t generate nonce value");
        return nullptr;
    }

    const SHA256::Hash::Ptr bodyHash = SHA256::getHash({
        {reinterpret_cast<const char*>(prevHash->data()), prevHash->length()},
        {reinterpret_cast<const char*>(nonce->data()), nonce->length()},
        data
    });

    if (!bodyHash)
    {
        Logger::error("Can\'t calculate block body hash");
        return nullptr;
    }

    const Crypto::Secp256k1::Signature::Ptr signature = _secp256k1.getSignature(bodyHash, header->getPrivateKey());

    if (!signature)
    {
        Logger::error("Can\'t get signature");
        return nullptr;
    }

    const SHA256::Hash::Ptr hash = SHA256::getHash({
        {reinterpret_cast<const char*>(prevHash->data()), prevHash->length()},
        {reinterpret_cast<const char*>(nonce->data()), nonce->length()},
        data,
        {reinterpret_cast<const char*>(signature->data()), signature->length()},
    });

    if (!hash)
    {
        Logger::error("Can\'t calculate block hash");
        return nullptr;
    }

    const Block::Container::Ptr container = std::make_shared<Block::Container>(
        hash,
        prevHash,
        nonce,
        data,
        signature);

    const Block::Ptr block(new Block(container));

    if (!chain.addBlock(block))
    {
        Logger::error("Can\'t add block");
        return nullptr;
    }

    return block;
}

Block::Ptr Manager::getBlock(const size_t chainId, const size_t index) const
{
    const Chain chain(makeStoragePath(chainId));

    return chain.getBlock(index);
}

bool Manager::getBlocks(const size_t chainId, BlockList& blocks) const
{
    const Chain chain(makeStoragePath(chainId));

    return chain.getBlocks(blocks);
}

bool Manager::removeChain(const size_t chainId) const
{
    const Chain chain(makeStoragePath(chainId));

    return chain.remove();
}

bool Manager::verifyChain(const size_t chainId) const
{
    const Chain chain(makeStoragePath(chainId));

    const Chain::Header::Ptr header = chain.getHeader();

    if (!header)
    {
        Logger::error("Can\'t get header");
        return false;
    }

    BlockList blocks;

    if (!chain.getBlocks(blocks))
    {
        Logger::error("Can\'t get blocks");
        return false;
    }

    for (size_t index = 0; index < blocks.size(); index++)
    {
        SHA256::Hash::Ptr prevHash;

        if (index == 0)
        {
            prevHash = SHA256::getHashN({
                header->getData(),
                {reinterpret_cast<const char*>(header->getPrivateKey()->data()), header->getPrivateKey()->length()},
                {reinterpret_cast<const char*>(header->getPublicKey()->data()), header->getPublicKey()->length()}
            });

            if (!prevHash)
            {
                Logger::error("Can\'t calculate header hash");
                return false;
            }
        }
        else
        {
            const Block::Ptr lastBlock = blocks[index - 1];

            prevHash = lastBlock->getData()->getHash();
        }

        const Block::Ptr block = blocks[index];

        const SHA256::Hash::Ptr bodyHash = SHA256::getHash({
            {reinterpret_cast<const char*>(prevHash->data()), prevHash->length()},
            {reinterpret_cast<const char*>(block->getData()->getNonce()->data()), block->getData()->getNonce()->length()},
            block->getData()->getData()
        });

        if (!_secp256k1.verifySignature(bodyHash, header->getPublicKey(), block->getData()->getSignature()))
        {
            Logger::error("Signature is not valid (Index: {})", index);
            return false;
        }

        const SHA256::Hash::Ptr hash = SHA256::getHash({
            {reinterpret_cast<const char*>(prevHash->data()), prevHash->length()},
            {reinterpret_cast<const char*>(block->getData()->getNonce()->data()), block->getData()->getNonce()->length()},
            block->getData()->getData(),
            {reinterpret_cast<const char*>(block->getData()->getSignature()->data()), block->getData()->getSignature()->length()},
        });

        if (!hash)
        {
            Logger::error("Can\'t calculate block hash (Index: {})", index);
            return false;
        }

        if (memcmp(block->getData()->getHash()->data(), hash->data(), hash->length()))
        {
            Logger::error("Hash is not valid (Index: {})", index);
            return false;
        }
    }

    return true;
}

Chain::Header::Ptr Manager::getChainHeader(const size_t chainId) const
{
    const Chain chain(makeStoragePath(chainId));

    const Chain::Header::Ptr header = chain.getHeader();

    if (!header)
    {
        Logger::error("Can\'t get header");
        return nullptr;
    }

    return header;
}

bool Manager::getChainInfo(const size_t chainId, size_t& version, size_t& index) const
{
    const Chain chain(makeStoragePath(chainId));

    const Chain::Header::Ptr header = chain.getHeader();

    if (!header)
    {
        Logger::error("Can\'t get header");
        return false;
    }

    version = header->getVersion();
    index = header->getIndex();

    return true;
}

std::string Manager::makeStoragePath(const size_t chainId) const
{
    const std::string& name = std::to_string(chainId) + ".blockchain";

    return std::filesystem::path(_storageDir) / std::filesystem::path(name);
}