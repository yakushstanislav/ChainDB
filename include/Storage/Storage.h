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

#include <string>
#include <vector>
#include <memory>

#ifdef USE_ROCKSDB
    #include <rocksdb/db.h>
    #include <rocksdb/write_batch.h>

    namespace DB = rocksdb;
#else
    #include <leveldb/db.h>
    #include <leveldb/write_batch.h>

    namespace DB = leveldb;
#endif

namespace Core::Storage
{

class Storage
{
public:
    struct KeyValue
    {
    public:
        typedef std::shared_ptr<KeyValue> Ptr;
        typedef std::string Data;

        KeyValue(const Data& key, const Data& value);
        ~KeyValue();

        Data getKey() const;
        Data getValue() const;

    private:
        Data _key;
        Data _value;
    };

    typedef std::vector<KeyValue> KeyValueList;

    explicit Storage(const std::string& path);
    ~Storage();

    Storage(Storage const&) = delete;
    void operator=(Storage const&) = delete;

    bool create();

    bool open();
    bool close();

    KeyValue::Ptr get(const KeyValue::Data& key) const;
    bool set(const KeyValueList& pairs) const;

    bool remove() const;

private:
    std::string _path;
    DB::DB* _db;
};

}