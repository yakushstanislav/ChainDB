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

#include "Defs.h"
#include "Application.h"

#include "System/Profiling.h"
#include "Crypto/SHA256.h"

using namespace Core;
using namespace Core::Network;

const size_t TIMEOUT_MS = 1000;

Application::Application() :
    _serverAddr("127.0.0.1"),
    _serverPort(8888),
    _timeout(1),
    _isPingRequest(false),
    _isCreateChainRequest(false),
    _isRemoveChainRequest(false),
    _isAddBlockRequest(false),
    _isGetBlockRequest(false),
    _isGetBlocksRequest(false),
    _isVerifyChainRequest(false),
    _isGetChainHeaderRequest(false),
    _isGetChainKeysRequest(false),
    _isGetChainInfoRequest(false),
    _chainId(1),
    _blockId(1),
    _data("{}")
{
}

Application::~Application()
{
}

bool Application::initialize(const int argc, const char* argv[])
{
    const HandlerMap handlers = {
        {"--addr", &_serverAddr},
        {"--port", &_serverPort},
        {"--timeout", &_timeout},
        {"--ping", &_isPingRequest},
        {"--create-chain", &_isCreateChainRequest},
        {"--remove-chain", &_isRemoveChainRequest},
        {"--add-block", &_isAddBlockRequest},
        {"--get-block", &_isGetBlockRequest},
        {"--get-blocks", &_isGetBlocksRequest},
        {"--verify-chain", &_isVerifyChainRequest},
        {"--get-header", &_isGetChainHeaderRequest},
        {"--get-keys", &_isGetChainKeysRequest},
        {"--get-info", &_isGetChainInfoRequest},
        {"--chain-id", &_chainId},
        {"--block-id", &_blockId},
        {"--password", &_password},
        {"--data", &_data}
    };

    if (!parseArgs(argc, argv, handlers))
    {
        return false;
    }

    initializeLogger();

    _client = new Client(_serverAddr, _serverPort, _timeout * TIMEOUT_MS);

    return true;
}

bool Application::run()
{
    if (_isPingRequest)
    {
        return ping();
    }
    else if (_isCreateChainRequest)
    {
        return createChain(_chainId, _data);
    }
    else if (_isRemoveChainRequest)
    {
        return removeChain(_chainId);
    }
    else if (_isAddBlockRequest)
    {
        return addBlock(_chainId, _data);
    }
    else if (_isGetBlockRequest)
    {
        return getBlock(_chainId, _blockId);
    }
    else if (_isGetBlocksRequest)
    {
        return getBlocks(_chainId);
    }
    else if (_isVerifyChainRequest)
    {
        return verifyChain(_chainId);
    }
    else if (_isGetChainHeaderRequest)
    {
        return getChainHeader(_chainId);
    }
    else if (_isGetChainKeysRequest)
    {
        return getChainKeys(_chainId);
    }
    else if (_isGetChainInfoRequest)
    {
        return getChainInfo(_chainId);
    }

    return true;
}

bool Application::ping() const
{
    Service::IPC::Request req;

    req.mutable_ping_request();

    return processRequest(req);
}

bool Application::createChain(const size_t chainId, const std::string& data) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_create_chain_request()->set_chain_id(chainId);
    req.mutable_create_chain_request()->set_data(data);

    return processRequest(req);
}

bool Application::removeChain(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_remove_chain_request()->set_chain_id(chainId);

    return processRequest(req);
}

bool Application::addBlock(const size_t chainId, const std::string& data) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_add_block_request()->set_chain_id(chainId);
    req.mutable_add_block_request()->set_data(data);

    return processRequest(req);
}

bool Application::getBlock(const size_t chainId, const size_t blockId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_get_block_request()->set_chain_id(chainId);
    req.mutable_get_block_request()->set_block_id(blockId);

    return processRequest(req);
}

bool Application::getBlocks(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_get_blocks_request()->set_chain_id(chainId);

    return processRequest(req);
}

bool Application::verifyChain(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_verify_chain_request()->set_chain_id(chainId);

    return processRequest(req);
}

bool Application::getChainHeader(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_get_chain_header_request()->set_chain_id(chainId);

    return processRequest(req);
}

bool Application::getChainKeys(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_get_chain_keys_request()->set_chain_id(chainId);

    return processRequest(req);
}

bool Application::getChainInfo(const size_t chainId) const
{
    Service::IPC::Request req;

    setAuthData(req.mutable_auth_data());

    req.mutable_get_chain_info_request()->set_chain_id(chainId);

    return processRequest(req);
}

void Application::setAuthData(Service::IPC::AuthData* data) const
{
    if (_password.empty())
        return;

    const Crypto::SHA256::Hash::Ptr hash = Crypto::SHA256::getHash({
        PASSWORD_SALT,
        _password
    });

    if (!hash)
    {
        Logger::error("Can\'t get password hash");
        return;
    }

    data->set_password_hash(hash->data(), hash->length());
}

bool Application::processRequest(const Service::IPC::Request& req) const
{
    const TrackTimeScope scope(__FUNCTION__);

    Service::IPC::Response resp;

    std::string data;
    if (!req.SerializeToString(&data))
    {
        Logger::error("Can\'t serialize message");
        return false;
    }

    Logger::info(req.ShortDebugString());

    const Message::Ptr msg(new Message(data.c_str(), data.length()));
    const Message::Ptr result = _client->sendMessage(msg);

    if (result)
    {
        if (!resp.ParseFromString(std::string(result->data(), result->length())))
        {
            Logger::error("Can\'t parse message");
            return false;
        }

        Logger::info(resp.ShortDebugString());

        return true;
    }

    return false;
}

void Application::initializeLogger() const
{
    spdlog::set_level(spdlog::level::debug);
}

int main(int argc, const char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Application app;

    if (!app.initialize(argc, argv))
    {
        return -1;
    }

    if (!app.run())
    {
        return -1;
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}