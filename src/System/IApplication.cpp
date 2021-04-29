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

#include <iostream>

#include "System/IApplication.h"

using namespace Core::System;

IApplication::~IApplication()
{
}

void IApplication::printHelp(const HandlerMap& handlers) const
{
    std::cout << "Usage:" << std::endl;

    for (auto& [key, value]: handlers)
    {
        const ArgData& data = value;
        std::string typeName = "undefined";

        if (std::holds_alternative<bool*>(data))
        {
            typeName = "bool";
        }
        else if (std::holds_alternative<int*>(data))
        {
            typeName = "int";
        }
        else if (std::holds_alternative<std::string*>(data))
        {
            typeName = "string";
        }

        std::cout << key << " " << typeName << std::endl;
    }
}

bool IApplication::parseArgs(const int argc, const char* argv[], const HandlerMap& handlers) const
{
    for (int i = 1; i < argc; i += 2)
    {
        if (i == argc - 1)
        {
            std::cerr << "Key \"" << argv[i] << "\" must have an argument" << std::endl;
            printHelp(handlers);
            return false;
        }

        const HandlerMap::const_iterator it = handlers.find(argv[i]);
        if (it != handlers.end())
        {
            const ArgData& data = it->second;

            if (std::holds_alternative<bool*>(data))
            {
                *std::get<bool*>(data) = std::string(argv[i + 1]) == "true";
            }
            else if (std::holds_alternative<int*>(data))
            {
                try
                {
                    *std::get<int*>(data) = std::stoi(argv[i + 1]);
                }
                catch (const std::invalid_argument& ex)
                {
                    #ifndef _TEST
                    std::cerr << "Invalid data for key \"" << argv[i] << "\"" << std::endl;
                    printHelp(handlers);
                    #endif

                    return false;
                }
            }
            else if (std::holds_alternative<std::string*>(data))
            {
                *std::get<std::string*>(data) = std::string(argv[i + 1]);
            }
            else
            {
                std::cerr << "Type is not supported" << std::endl;
            }
        }
        else
        {
            std::cerr << "Key \"" << argv[i] << "\" is not supported" << std::endl;
            printHelp(handlers);
            return false;
        }
    }

    return true;
}