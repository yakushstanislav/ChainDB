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
#include "Network/Client/Client.h"

using namespace Core::Network;

Client::Client(const std::string& addr, const size_t port, const size_t timeout) :
    _addr(addr),
    _port(port),
    _timeout(timeout)
{
}

Client::Client(const size_t port, const size_t timeout) :
    _addr("127.0.0.1"),
    _port(port),
    _timeout(timeout)
{
}

Client::~Client()
{
}

Core::Network::Message::Ptr Client::sendMessage(const Message::Ptr msg) const
{
    zmq_msg_t reqMsg, respMsg;
    zmq_pollitem_t items[1];
    Core::Network::Message::Ptr resp = nullptr;

    void* context = zmq_ctx_new();
    if (!context)
    {
        Logger::error("ZMQ context error");
        return resp;
    }

    void* socket = zmq_socket(context, ZMQ_REQ);
    if (!socket)
    {
        Logger::error("ZMQ socket error");
        zmq_ctx_destroy(context);
        return resp;
    }

    int err = zmq_connect(socket, makeEndpointPath().c_str());
    if (err != 0)
    {
        Logger::error("ZMQ connect error: {}", zmq_strerror(err));
        goto out;
    }

    err = zmq_msg_init_size(&reqMsg, msg->length());
    if (err != 0)
    {
        Logger::error("ZMQ msg init error: {}", zmq_strerror(err));
        goto out;
    }

    std::memcpy(zmq_msg_data(&reqMsg), msg->data(), msg->length());

    err = zmq_msg_send(&reqMsg, socket, 0);
    if (err == -1)
    {
        Logger::error("ZMQ send error: {}", zmq_strerror(err));
        zmq_msg_close(&reqMsg);
        goto out;
    }

    zmq_msg_close(&reqMsg);

    items[0] = {socket, 0, ZMQ_POLLIN, 0};

    err = zmq_poll(items, 1, _timeout * ZMQ_POLL_MSEC);
    if (err == -1)
    {
        Logger::error("ZMQ pool error: {}", zmq_strerror(err));
        goto out;
    }

    if (items[0].revents & ZMQ_POLLIN)
    {
        err = zmq_msg_init(&respMsg);
        if (err != 0)
        {
            Logger::error("ZMQ msg init error: {}", zmq_strerror(err));
            goto out;
        }

        err = zmq_msg_recv(&respMsg, socket, 0);
        if (err == -1)
        {
            Logger::error("ZMQ recv error: {}", zmq_strerror(err));
            zmq_msg_close(&respMsg);
            goto out;
        }

        if (zmq_msg_size(&respMsg))
        {
            resp = std::make_shared<Message>(static_cast<char*>(zmq_msg_data(&respMsg)), zmq_msg_size(&respMsg));
        }

        zmq_msg_close(&respMsg);
    }

out:
    zmq_close(socket);
    zmq_ctx_destroy(context);

    return resp;
}

std::string Client::makeEndpointPath() const
{
    std::ostringstream ss;
    ss << "tcp://" << _addr << ":" << _port;
    return ss.str();
}