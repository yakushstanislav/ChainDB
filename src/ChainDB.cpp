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

#include <csignal>
#include <memory>

#include "Defs.h"
#include "ChainDB.h"
#include "Handler.h"
#include "Storage/Manager.h"
#include "Crypto/Random.h"

using namespace Core;
using namespace Core::Storage;
using namespace Core::Network;
using namespace Core::Crypto;

ChainDB& ChainDB::instance()
{
    static ChainDB instance;

    return instance;
}

ChainDB::ChainDB() :
    _daemonize(true),
    _logPath("chain_db_service.log"),
    _serverPort(8888),
    _server(nullptr)
{
}

ChainDB::~ChainDB()
{
}

bool ChainDB::initialize(const int argc, const char* argv[])
{
    const HandlerMap handlers = {
        {"--daemonize", &_daemonize},
        {"--log-path", &_logPath},
        {"--storage-path", &_storageDir},
        {"--password", &_password},
        {"--port", &_serverPort}
    };

    initializeSignalHandler();

    if (!parseArgs(argc, argv, handlers))
    {
        return false;
    }

    if (_daemonize)
    {
        daemonize();
    }

    initializeLogger();

    if (!initializeRandomGenerator())
    {
        Logger::error("Can\'t initialize random generator");
        return false;
    }

    return true;
}

bool ChainDB::run()
{
    Manager manager(_storageDir);
    Handler handler(manager, _password);

    Logger::info("Start (Version: {})...", SERVICE_VERSION);

    _server = new Server(_serverPort, handler);
    _server->start();
    _server->join();

    Logger::info("Shutdown...");
    Logger::shutdown();

    return true;
}

void ChainDB::stop()
{
    if (_server)
    {
        return _server->stop();
    }
}

void ChainDB::initializeSignalHandler() const
{
    signal(SIGINT, ChainDB::signalHandler);
    signal(SIGTERM, ChainDB::signalHandler);
}

void ChainDB::initializeLogger() const
{
    const std::vector<spdlog::sink_ptr>& sinks = {
        std::make_shared<spdlog::sinks::stderr_color_sink_mt>(),
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_logPath,
            LOG_MAX_FILE_SIZE, LOG_MAX_FILE_COUNT)
    };

    auto logger = std::make_shared<spdlog::logger>(LOG_NAME, std::begin(sinks), std::end(sinks));

    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);

    spdlog::set_default_logger(logger);
}

bool ChainDB::initializeRandomGenerator() const
{
    if (!Random::status())
    {
        return false;
    }

    if (!Random::poll())
    {
        return false;
    }

    return true;
}

void ChainDB::signalHandler(const int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
    {
        ChainDB::instance().stop();
    }
}