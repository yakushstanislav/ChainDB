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

#include <chrono>
#include <thread>
#include <ostream>
#include <filesystem>

#include <spdlog/sinks/ostream_sink.h>

#include "System/Logger.h"

class BaseTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        setupLogger();
    }

    void TearDown() override
    {
    }

    void setupLogger()
    {
        const std::vector<spdlog::sink_ptr>& sinks = {
            std::make_shared<spdlog::sinks::ostream_sink_st>(_stream)
        };

        auto logger = std::make_shared<spdlog::logger>("test", std::begin(sinks), std::end(sinks));

        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);

        logger->set_pattern("%v");

        spdlog::set_default_logger(logger);
    }

    void waitSec(const size_t value = 1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(value));
    }

    void waitMs(const size_t value)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(value));
    }

    std::string makeTempPath() const
    {
        const std::string& name = std::to_string(time(NULL)) + std::to_string(rand());

        return std::filesystem::temp_directory_path() / name;
    }

    std::string createTempDirectory() const
    {
        const std::string& path = makeTempPath();

        std::filesystem::create_directory(path);

        return path;
    }

    bool removeDirectory(const std::string& path) const
    {
        return std::filesystem::remove_all(path);
    }

    std::string getLogData()
    {
        const std::string& log = _stream.str();

        _stream.str("");
        _stream.clear();

        return log;
    }

    std::ostringstream _stream;
};
