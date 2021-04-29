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

#include "BaseTest.h"

#include "service.pb.h"

#include "Handler.h"
#include "Storage/Manager.h"
#include "Network/Client/Client.h"
#include "Network/Server/Server.h"
#include "Crypto/SHA256.h"

class HandlerTest : public BaseTest
{
    const size_t PORT = 10750;
    const size_t TIMEOUT_MS = 2000;

    void SetUp() override
    {
        setupLogger();
        setupClient();
    }

    void TearDown() override
    {
        if (_client)
        {
            delete _client;
        }

        if (_server)
        {
            stopServer();
        }

        if (_tempPath.length())
        {
            removeDirectory(_tempPath);
        }
    }

    void setupClient()
    {
        _client = new Core::Network::Client(PORT, TIMEOUT_MS);
    }

public:
    std::string tempDirectory()
    {
        _tempPath = createTempDirectory();

        return _tempPath;
    }

    void startServer(Core::Handler& handler)
    {
        _server = new Core::Network::Server(PORT, handler);

        _server->start();
    }

    void stopServer()
    {
        _server->stop();
        _server->join();

        delete _server;
    }

    bool sendRequest(const Service::IPC::Request& req, Service::IPC::Response& resp) const
    {
        std::string data;

        if (!req.SerializeToString(&data))
        {
            return false;
        }

        const Core::Network::Message::Ptr msg(new Core::Network::Message(data.c_str(), data.length()));
        const Core::Network::Message::Ptr result = _client->sendMessage(msg);

        if (result)
        {
            if (!resp.ParseFromString(std::string(result->data(), result->length())))
            {
                return false;
            }

            return true;
        }

        return false;
    }

    void setAuthData(Service::IPC::AuthData* data, const std::string& password) const
    {
        const Core::Crypto::SHA256::Hash::Ptr hash = Core::Crypto::SHA256::getHash({
            PASSWORD_SALT,
            password
        });

        EXPECT_TRUE(hash);

        data->set_password_hash(hash->data(), hash->length());
    }

private:
    std::string _tempPath;

    Core::Network::Client* _client = nullptr;
    Core::Network::Server* _server = nullptr;
};

TEST_F(HandlerTest, Empty)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::NOT_SUPPORTED);
}

TEST_F(HandlerTest, CheckAuth)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager, "password");

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    setAuthData(req.mutable_auth_data(), "password");

    req.mutable_ping_request();

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
}

TEST_F(HandlerTest, AuthNoPassword)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager, "password");

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    req.mutable_ping_request();

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::NOT_AUTHORIZED);
}

TEST_F(HandlerTest, AuthInvalidPassword)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager, "password");

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    setAuthData(req.mutable_auth_data(), "secret");

    req.mutable_ping_request();

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::NOT_AUTHORIZED);
}

TEST_F(HandlerTest, Ping)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    req.mutable_ping_request();

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
}

TEST_F(HandlerTest, CreateChain)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    req.mutable_create_chain_request()->set_chain_id(1);
    req.mutable_create_chain_request()->set_data("data");

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
}

TEST_F(HandlerTest, CreateChainTwice)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    Service::IPC::Request req;
    Service::IPC::Response resp;

    req.mutable_create_chain_request()->set_chain_id(1);
    req.mutable_create_chain_request()->set_data("data");

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

    EXPECT_TRUE(sendRequest(req, resp));

    EXPECT_TRUE(resp.has_status());

    EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);
}

TEST_F(HandlerTest, RemoveChain)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_remove_chain_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }
}

TEST_F(HandlerTest, AddBlock)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(1);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.add_block_response().has_block());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(2);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);

        EXPECT_FALSE(resp.add_block_response().has_block());
    }
}

TEST_F(HandlerTest, GetBlock)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(1);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.add_block_response().has_block());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_block_request()->set_chain_id(1);
        req.mutable_get_block_request()->set_block_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.get_block_response().has_block());
    }
}

TEST_F(HandlerTest, GetBlocks)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.has_status());
    }

    for (size_t i = 0; i < 8; i++)
    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(1);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.add_block_response().has_block());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_blocks_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_EQ(resp.get_blocks_response().blocks_size(), 8);
    }
}

TEST_F(HandlerTest, VerifyChain)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.has_status());
    }

    for (size_t i = 0; i < 8; i++)
    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(1);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.add_block_response().has_block());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_verify_chain_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_verify_chain_request()->set_chain_id(2);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);
    }
}

TEST_F(HandlerTest, GetChainHeader)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_header_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_EQ(resp.get_chain_header_response().header().version(), DB_VERSION);
        EXPECT_EQ(resp.get_chain_header_response().header().index(), 0);

        EXPECT_TRUE(resp.get_chain_header_response().header().data().size());
        EXPECT_TRUE(resp.get_chain_header_response().header().private_key().size());
        EXPECT_TRUE(resp.get_chain_header_response().header().public_key().size());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_header_request()->set_chain_id(2);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);
    }
}

TEST_F(HandlerTest, GetChainKeys)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_keys_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.get_chain_keys_response().private_key().size());
        EXPECT_TRUE(resp.get_chain_keys_response().public_key().size());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_keys_request()->set_chain_id(2);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);
    }
}

TEST_F(HandlerTest, GetChainInfo)
{
    Core::Storage::Manager manager(tempDirectory());
    Core::Handler handler(manager);

    startServer(handler);

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_create_chain_request()->set_chain_id(1);
        req.mutable_create_chain_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_add_block_request()->set_chain_id(1);
        req.mutable_add_block_request()->set_data("data");

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_TRUE(resp.add_block_response().has_block());
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_info_request()->set_chain_id(1);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::SUCCESS);

        EXPECT_EQ(resp.get_chain_info_response().chain_id(), 1);
        EXPECT_EQ(resp.get_chain_info_response().version(), DB_VERSION);
        EXPECT_EQ(resp.get_chain_info_response().index(), 1);
    }

    {
        Service::IPC::Request req;
        Service::IPC::Response resp;

        req.mutable_get_chain_info_request()->set_chain_id(2);

        EXPECT_TRUE(sendRequest(req, resp));

        EXPECT_TRUE(resp.has_status());

        EXPECT_EQ(resp.status().status(), Core::Handler::ERROR);
    }
}