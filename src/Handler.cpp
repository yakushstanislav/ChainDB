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

#include "System/Logger.h"
#include "Crypto/SHA256.h"

#include "Defs.h"
#include "Handler.h"

using namespace Core;
using namespace Core::Network;

Handler::Handler(Storage::Manager& manager, const std::string& password) :
    _manager(manager),
    _password(password)
{
}

Handler::~Handler()
{
}

Message::Ptr Handler::handleMessage(const Message::Ptr msg) const
{
    Service::IPC::Request req;

    if (!req.ParseFromString(std::string(msg->data(), msg->length())))
    {
        return makeStatus(DATA_ERROR, "Can\'t parse data");
    }

    if (!_password.empty())
    {
        if (!req.has_auth_data())
        {
            return makeStatus(NOT_AUTHORIZED, "No authorization data");
        }

        if (!checkAuth(req.auth_data()))
        {
            return makeStatus(NOT_AUTHORIZED, "Invalid password");
        }
    }

    if (req.has_ping_request())
    {
        return handlePingRequest(req.ping_request());
    }
    else if (req.has_create_chain_request())
    {
        return handleCreateChainRequest(req.create_chain_request());
    }
    else if (req.has_remove_chain_request())
    {
        return handleRemoveChainRequest(req.remove_chain_request());
    }
    else if (req.has_add_block_request())
    {
        return handleAddBlockRequest(req.add_block_request());
    }
    else if (req.has_get_block_request())
    {
        return handleGetBlockRequest(req.get_block_request());
    }
    else if (req.has_get_blocks_request())
    {
        return handleGetBlocksRequest(req.get_blocks_request());
    }
    else if (req.has_verify_chain_request())
    {
        return handleVerifyChainRequest(req.verify_chain_request());
    }
    else if (req.has_get_chain_header_request())
    {
        return handleGetChainHeaderRequest(req.get_chain_header_request());
    }
    else if (req.has_get_chain_keys_request())
    {
        return handleGetChainKeysRequest(req.get_chain_keys_request());
    }
    else if (req.has_get_chain_info_request())
    {
        return handleGetChainInfoRequest(req.get_chain_info_request());
    }

    return makeStatus(NOT_SUPPORTED, "Method isn\'t supported");
}

Network::Message::Ptr Handler::handlePingRequest(const Service::IPC::PingRequest&) const
{
    Logger::info("Handle ping request");

    return makeStatus(SUCCESS);
}

Network::Message::Ptr Handler::handleCreateChainRequest(const Service::IPC::CreateChainRequest& req) const
{
    Logger::info("Handle create chain request (Chain ID: {})", req.chain_id());

    if (req.data().size() > MAX_DATA_LENGTH)
    {
        return makeStatus(DATA_ERROR, "Can\'t create chain (Data field size is too large)");
    }

    const Storage::Chain::Ptr chain = _manager.createChain(req.chain_id(), req.data());

    if (!chain)
    {
        return makeStatus(ERROR, "Can\'t create chain");
    }

    return makeStatus(SUCCESS);
}

Network::Message::Ptr Handler::handleRemoveChainRequest(const Service::IPC::RemoveChainRequest& req) const
{
    Logger::info("Handle remove chain request (Chain ID: {})", req.chain_id());

    if (!_manager.removeChain(req.chain_id()))
    {
        return makeStatus(ERROR, "Can\'t remove chain");
    }

    return makeStatus(SUCCESS);
}

Network::Message::Ptr Handler::handleAddBlockRequest(const Service::IPC::AddBlockRequest& req) const
{
    Logger::info("Handle add block request (Chain ID: {})", req.chain_id());

    if (req.data().size() > MAX_DATA_LENGTH)
    {
        return makeStatus(DATA_ERROR, "Can\'t create chain (Data field size is too large)");
    }

    const Storage::Block::Ptr block = _manager.addBlock(req.chain_id(), req.data());

    if (!block)
    {
        return makeStatus(ERROR, "Can\'t add block");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    setBlockData(resp.mutable_add_block_response()->mutable_block(), block);

    return makeResponse(resp);
}

Network::Message::Ptr Handler::handleGetBlockRequest(const Service::IPC::GetBlockRequest& req) const
{
    Logger::info("Handle get block request (Chain ID: {}, Block ID: {})", req.chain_id(), req.block_id());

    const Storage::Block::Ptr block = _manager.getBlock(req.chain_id(), req.block_id());

    if (!block)
    {
        return makeStatus(ERROR, "Can\'t get block");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    setBlockData(resp.mutable_get_block_response()->mutable_block(), block);

    return makeResponse(resp);
}

Network::Message::Ptr Handler::handleGetBlocksRequest(const Service::IPC::GetBlocksRequest& req) const
{
    Logger::info("Handle get blocks request (Chain ID: {})", req.chain_id());

    Storage::Manager::BlockList blocks;

    if (!_manager.getBlocks(req.chain_id(), blocks))
    {
        return makeStatus(ERROR, "Can\'t get blocks");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    for (const Storage::Block::Ptr block : blocks)
    {
        setBlockData(resp.mutable_get_blocks_response()->add_blocks(), block);
    }

    return makeResponse(resp);
}

Network::Message::Ptr Handler::handleVerifyChainRequest(const Service::IPC::VerifyChainRequest& req) const
{
    Logger::info("Handle verify chain request (Chain ID: {})", req.chain_id());

    if (!_manager.verifyChain(req.chain_id()))
    {
        return makeStatus(ERROR, "Chain is not valid");
    }

    return makeStatus(SUCCESS);
}

Network::Message::Ptr Handler::handleGetChainHeaderRequest(const Service::IPC::GetChainHeaderRequest& req) const
{
    Logger::info("Handle get chain header request (Chain ID: {})", req.chain_id());

    Storage::Chain::Header::Ptr header = _manager.getChainHeader(req.chain_id());

    if (!header)
    {
        return makeStatus(ERROR, "Can\'t get header");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    resp.mutable_get_chain_header_response()->mutable_header()->set_version(header->getVersion());
    resp.mutable_get_chain_header_response()->mutable_header()->set_index(header->getIndex());

    resp.mutable_get_chain_header_response()->mutable_header()->set_data(
        header->getData().data(),
        header->getData().length()
    );

    resp.mutable_get_chain_header_response()->mutable_header()->set_private_key(
        header->getPrivateKey()->data(),
        header->getPrivateKey()->length()
    );

    resp.mutable_get_chain_header_response()->mutable_header()->set_public_key(
        header->getPublicKey()->data(),
        header->getPublicKey()->length()
    );

    return makeResponse(resp);
}

Network::Message::Ptr Handler::handleGetChainKeysRequest(const Service::IPC::GetChainKeysRequest& req) const
{
    Logger::info("Handle get chain keys request (Chain ID: {})", req.chain_id());

    Storage::Chain::Header::Ptr header = _manager.getChainHeader(req.chain_id());

    if (!header)
    {
        return makeStatus(ERROR, "Can\'t get header");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    resp.mutable_get_chain_keys_response()->set_private_key(
        header->getPrivateKey()->data(),
        header->getPrivateKey()->length()
    );

    resp.mutable_get_chain_keys_response()->set_public_key(
        header->getPublicKey()->data(),
        header->getPublicKey()->length()
    );

    return makeResponse(resp);
}

Network::Message::Ptr Handler::handleGetChainInfoRequest(const Service::IPC::GetChainInfoRequest& req) const
{
    Logger::info("Handle get chain info request (Chain ID: {})", req.chain_id());

    size_t version = 0;
    size_t index = 0;

    if (!_manager.getChainInfo(req.chain_id(), version, index))
    {
        return makeStatus(ERROR, "Can\'t get chain info");
    }

    Service::IPC::Response resp;

    resp.mutable_status()->set_status(SUCCESS);

    resp.mutable_get_chain_info_response()->set_chain_id(req.chain_id());
    resp.mutable_get_chain_info_response()->set_version(version);
    resp.mutable_get_chain_info_response()->set_index(index);

    return makeResponse(resp);
}

Network::Message::Ptr Handler::makeResponse(const Service::IPC::Response& resp) const
{
    std::string data;

    if (!resp.SerializeToString(&data))
    {
        Logger::error("Can\'t serialize response");
        return nullptr;
    }

    return std::make_shared<Network::Message>(data.c_str(), data.length());
}

Network::Message::Ptr Handler::makeStatus(const Status status, const std::string& text) const
{
    Service::IPC::Response resp;

    if (status != SUCCESS && !text.empty())
    {
        Logger::error(text);
    }

    resp.mutable_status()->set_status(status);

    if (!text.empty())
    {
        resp.mutable_status()->set_message(text);
    }

    std::string data;
    if (!resp.SerializeToString(&data))
    {
        Logger::error("Can\'t serialize status message");
        return nullptr;
    }

    return std::make_shared<Network::Message>(data.c_str(), data.length());
}

bool Handler::checkAuth(const Service::IPC::AuthData& data) const
{
    if (data.password_hash().size() != sizeof(Crypto::SHA256::Hash::Value))
    {
        Logger::error("Invalid password hash");
        return false;
    }

    const Crypto::SHA256::Hash::Ptr hash = Crypto::SHA256::getHash({
        PASSWORD_SALT,
        _password
    });

    if (!hash)
    {
        Logger::error("Can\'t get password hash");
        return false;
    }

    return !memcmp(hash->data(), data.password_hash().c_str(), hash->length());
}

void Handler::setBlockData(Service::Blockchain::Block* data, const Storage::Block::Ptr block) const
{
    data->set_hash(
        block->getData()->getHash()->data(),
        block->getData()->getHash()->length()
    );

    data->set_prev_hash(
        block->getData()->getPrevHash()->data(),
        block->getData()->getPrevHash()->length()
    );

    data->set_nonce(
        block->getData()->getNonce()->data(),
        block->getData()->getNonce()->length()
    );

    data->set_data(block->getData()->getData());

    data->set_signature(
        block->getData()->getSignature()->data(),
        block->getData()->getSignature()->length()
    );
}
