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

#include "service.pb.h"

#include <Network/Server/IHandler.h>
#include "Storage/Manager.h"

namespace Core
{

class Handler : public Network::IHandler
{
public:
    enum Status
    {
        SUCCESS = 0,
        ERROR = 1,
        DATA_ERROR = 2,
        NOT_SUPPORTED = 3,
        NOT_AUTHORIZED = 4
    };

    Handler(Storage::Manager& manager, const std::string& password = "");
    ~Handler();

    Network::Message::Ptr handleMessage(const Network::Message::Ptr msg) const override;

private:
    Network::Message::Ptr handlePingRequest(const Service::IPC::PingRequest&) const;
    Network::Message::Ptr handleCreateChainRequest(const Service::IPC::CreateChainRequest& req) const;
    Network::Message::Ptr handleRemoveChainRequest(const Service::IPC::RemoveChainRequest& req) const;
    Network::Message::Ptr handleAddBlockRequest(const Service::IPC::AddBlockRequest& req) const;
    Network::Message::Ptr handleGetBlockRequest(const Service::IPC::GetBlockRequest& req) const;
    Network::Message::Ptr handleGetBlocksRequest(const Service::IPC::GetBlocksRequest& req) const;
    Network::Message::Ptr handleVerifyChainRequest(const Service::IPC::VerifyChainRequest& req) const;
    Network::Message::Ptr handleGetChainHeaderRequest(const Service::IPC::GetChainHeaderRequest& req) const;
    Network::Message::Ptr handleGetChainKeysRequest(const Service::IPC::GetChainKeysRequest& req) const;
    Network::Message::Ptr handleGetChainInfoRequest(const Service::IPC::GetChainInfoRequest& req) const;

    Network::Message::Ptr makeResponse(const Service::IPC::Response& resp) const;
    Network::Message::Ptr makeStatus(const Status status, const std::string& text = "") const;

private:
    bool checkAuth(const Service::IPC::AuthData& data) const;

    void setBlockData(Service::Blockchain::Block* data, const Storage::Block::Ptr block) const;

private:
    Storage::Manager& _manager;
    std::string _password;
};

}