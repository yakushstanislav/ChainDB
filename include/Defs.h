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

#ifndef SERVICE_VERSION
    #define SERVICE_VERSION "0.0"
#endif

#ifndef LOG_NAME
    #define LOG_NAME "chain_db"
#endif

#ifndef LOG_MAX_FILE_SIZE
    #define LOG_MAX_FILE_SIZE 20000000
#endif

#ifndef LOG_MAX_FILE_COUNT
    #define LOG_MAX_FILE_COUNT 20
#endif

#ifndef DB_VERSION
    #define DB_VERSION 0
#endif

#ifndef MAX_DATA_LENGTH
    #define MAX_DATA_LENGTH 8192
#endif

#ifndef NONCE_LENGTH
    #define NONCE_LENGTH 8
#endif

#ifndef PASSWORD_SALT
    #define PASSWORD_SALT "EMPTY_SALT/"
#endif