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

#include "Network/Server/IHandler.h"
#include "Network/Server/Server.h"
#include "Network/Client/Client.h"

class TestHandler : public Core::Network::IHandler
{
public:
    TestHandler()
    {
    }

    ~TestHandler()
    {
    }

    Core::Network::Message::Ptr handleMessage(const Core::Network::Message::Ptr msg) const override
    {
        if (std::string(msg->data(), msg->length()) == "ping")
        {
            const std::string& resp = "pong";

            return std::make_shared<Core::Network::Message>(resp.c_str(), resp.length());
        }

        return nullptr;
    }
};

class NetworkTest : public BaseTest
{
};

TEST_F(NetworkTest, RunServer)
{
    TestHandler handler;

    Core::Network::Server server(25750, handler);

    server.start();
    server.stop();
    server.join();

    EXPECT_EQ(getLogData(), "Run server (Port: 25750)...\n");
}

TEST_F(NetworkTest, RunServerPortTwice)
{
    TestHandler handler;

    Core::Network::Server server1(25750, handler);
    Core::Network::Server server2(25750, handler);

    server1.start();

    waitSec();

    EXPECT_EQ(getLogData(), "Run server (Port: 25750)...\n");

    server2.start();
    server2.stop();
    server2.join();

    waitSec();

    EXPECT_EQ(getLogData(), "Run server (Port: 25750)...\n"
                            "ZMQ bind error: Unknown error -1\n");

    server1.stop();
    server1.join();
}

TEST_F(NetworkTest, HandleMessage)
{
    TestHandler handler;

    Core::Network::Server server(25750, handler);
    Core::Network::Client client(25750);

    const std::string& request = "ping";

    server.start();

    const Core::Network::Message::Ptr resp = client.sendMessage(
        std::make_shared<Core::Network::Message>(request.c_str(),
        request.length()));

    EXPECT_TRUE(resp);
    EXPECT_EQ(std::string(resp->data(), resp->length()), "pong");

    server.stop();
    server.join();

    EXPECT_EQ(getLogData(), "Run server (Port: 25750)...\n");
}

TEST_F(NetworkTest, HandleMessageInvalid)
{
    TestHandler handler;

    Core::Network::Server server(25750, handler);
    Core::Network::Client client(25750, 1);

    const std::string& request = "test";

    server.start();

    const Core::Network::Message::Ptr resp = client.sendMessage(
        std::make_shared<Core::Network::Message>(request.c_str(),
        request.length()));

    EXPECT_FALSE(resp);

    server.stop();
    server.join();

    EXPECT_EQ(getLogData(), "Run server (Port: 25750)...\n");
}