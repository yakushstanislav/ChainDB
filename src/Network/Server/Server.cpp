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

#include <sstream>
#include <cstring>
#include <czmq.h>

#include "System/Logger.h"
#include "Network/Message.h"
#include "Network/Server/Server.h"

using namespace Core::Network;

const size_t ZMQ_POOL_TIMEOUT = 5;

Server::Server(const size_t port, IHandler& handler) :
    _port(port),
    _handler(handler),
    _isStopped(ATOMIC_FLAG_INIT)
{
    _isStopped.test_and_set();
}

Server::~Server()
{
}

void Server::start()
{
    _thread = std::thread(&Server::process, this);
}

void Server::stop()
{
    _isStopped.clear();
}

void Server::join()
{
    _thread.join();
}

void Server::process()
{
    Logger::info("Run server (Port: {})...", _port);

    void* context = zmq_ctx_new();
    if (!context)
    {
        Logger::error("ZMQ context error");
        return;
    }

    void* socket = zmq_socket(context, ZMQ_REP);
    if (!socket)
    {
        Logger::error("ZMQ socket error");
        zmq_ctx_destroy(context);
        return;
    }

    int err = zmq_bind(socket, makeEndpointPath().c_str());
    if (err != 0)
    {
        Logger::error("ZMQ bind error: {}", zmq_strerror(err));
        goto out;
    }

    while (_isStopped.test_and_set())
    {
        zmq_pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};

        err = zmq_poll(items, 1, ZMQ_POOL_TIMEOUT * ZMQ_POLL_MSEC);
        if (err == -1)
        {
            Logger::error("ZMQ pool error: {}", zmq_strerror(err));
            break;
        }

        if (items[0].revents & ZMQ_POLLIN)
        {
            zmq_msg_t reqMsg, respMsg;

            err = zmq_msg_init(&reqMsg);
            if (err != 0)
            {
                Logger::error("ZMQ msg init error: {}", zmq_strerror(err));
                break;
            }

            err = zmq_msg_recv(&reqMsg, socket, 0);
            if (err == -1)
            {
                Logger::error("ZMQ recv error: {}", zmq_strerror(err));
                zmq_msg_close(&reqMsg);
                break;
            }

            const Message::Ptr req(new Message(static_cast<char*>(zmq_msg_data(&reqMsg)), zmq_msg_size(&reqMsg)));
            const Message::Ptr resp = _handler.handleMessage(req);

            zmq_msg_close(&reqMsg);

            if (resp)
            {
                err = zmq_msg_init_size(&respMsg, resp->length());
                if (err != 0)
                {
                    Logger::error("ZMQ msg init error: {}", zmq_strerror(err));
                    break;
                }

                std::memcpy(zmq_msg_data(&respMsg), resp->data(), resp->length());

                err = zmq_msg_send(&respMsg, socket, 0);
                if (err == -1)
                {
                    Logger::error("ZMQ send error: {}", zmq_strerror(err));
                }

                zmq_msg_close(&respMsg);
            }
        }
    }

out:
    zmq_close(socket);
    zmq_ctx_destroy(context);
}

std::string Server::makeEndpointPath() const
{
    std::ostringstream ss;
    ss << "tcp://*:" << _port;
    return ss.str();
}