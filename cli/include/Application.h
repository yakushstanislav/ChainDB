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

#include "System/Logger.h"
#include "System/IApplication.h"
#include "Network/Client/Client.h"

namespace Core
{

class Application : public System::IApplication
{
public:
    Application();
    ~Application();

    Application(Application const&) = delete;
    void operator=(Application const&) = delete;

    bool initialize(const int argc, const char* argv[]) override;
    bool run() override;

private:
    bool ping() const;
    bool createChain(const size_t chainId, const std::string& data) const;
    bool removeChain(const size_t chainId) const;
    bool addBlock(const size_t chainId, const std::string& data) const;
    bool getBlock(const size_t chainId, const size_t blockId) const;
    bool getBlocks(const size_t chainId) const;
    bool verifyChain(const size_t chainId) const;
    bool getChainHeader(const size_t chainId) const;
    bool getChainKeys(const size_t chainId) const;
    bool getChainInfo(const size_t chainId) const;

    void setAuthData(Service::IPC::AuthData* data) const;

    bool processRequest(const Service::IPC::Request& req) const;

    void initializeLogger() const;

private:
    std::string _serverAddr;

    int _serverPort;
    int _timeout;

    bool _isPingRequest;
    bool _isCreateChainRequest;
    bool _isRemoveChainRequest;
    bool _isAddBlockRequest;
    bool _isGetBlockRequest;
    bool _isGetBlocksRequest;
    bool _isVerifyChainRequest;
    bool _isGetChainHeaderRequest;
    bool _isGetChainKeysRequest;
    bool _isGetChainInfoRequest;

    int _chainId;
    int _blockId;

    std::string _password;
    std::string _data;

    Network::Client* _client;
};

}