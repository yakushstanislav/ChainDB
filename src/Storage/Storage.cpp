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

#include "Storage/Storage.h"
#include "System/Logger.h"

using namespace Core::Storage;

Storage::KeyValue::KeyValue(const Data& key, const Data& value) :
    _key(key),
    _value(value)
{
}

Storage::KeyValue::~KeyValue()
{
}

Storage::KeyValue::Data Storage::KeyValue::getKey() const
{
    return _key;
}

Storage::KeyValue::Data Storage::KeyValue::getValue() const
{
    return _value;
}

Storage::Storage(const std::string& path) :
    _path(path),
    _db(nullptr)
{
}

Storage::~Storage()
{
    if (_db)
    {
        delete _db;
    }
}

bool Storage::create()
{
    if (_db)
    {
        Logger::error("DB already open (Path: {})", _path);
        return false;
    }

    DB::Options options;

    options.create_if_missing = true;
    options.error_if_exists = true;
    options.paranoid_checks = true;
    options.compression = DB::kNoCompression;

    const DB::Status status = DB::DB::Open(options, _path, &_db);

    if (!status.ok())
    {
        Logger::error("Can\'t create DB ({})", status.ToString());
        return false;
    }

    return true;
}

bool Storage::open()
{
    if (_db)
    {
        Logger::error("DB already open (Path: {})", _path);
        return false;
    }

    DB::Options options;

    options.paranoid_checks = true;
    options.compression = DB::kNoCompression;

    const DB::Status status = DB::DB::Open(options, _path, &_db);

    if (!status.ok())
    {
        Logger::error("Can\'t open DB ({})", status.ToString());
        return false;
    }

    return true;
}

bool Storage::close()
{
    if (!_db)
    {
        Logger::error("DB is not open (Path: {})", _path);
        return false;
    }

    delete _db;

    _db = nullptr;

    return true;
}

Storage::KeyValue::Ptr Storage::get(const KeyValue::Data& key) const
{
    if (!_db)
    {
        Logger::error("DB is not open (Path: {})", _path);
        return nullptr;
    }

    DB::ReadOptions readOptions;

    readOptions.verify_checksums = true;

    std::string value;

    const DB::Status status = _db->Get(readOptions, key, &value);

    if (!status.ok())
    {
        Logger::error("Can\'t get value ({})", status.ToString());
        return nullptr;
    }

    return std::make_shared<Storage::KeyValue>(key, value);
}

bool Storage::set(const KeyValueList& pairs) const
{
    if (!_db)
    {
        Logger::error("DB is not open (Path: {})", _path);
        return false;
    }

    DB::WriteBatch batch;

    for (const KeyValue& pair : pairs)
    {
        batch.Put(pair.getKey(), pair.getValue());
    }

    DB::WriteOptions writeOptions;

    writeOptions.sync = true;

    const DB::Status status = _db->Write(writeOptions, &batch);

    if (!status.ok())
    {
        Logger::error("Can\'t set value ({})", status.ToString());
        return false;
    }

    return true;
}

bool Storage::remove() const
{
    const DB::Status status = DB::DestroyDB(_path, DB::Options());

    if (!status.ok())
    {
        Logger::error("Can\'t remove DB ({})", status.ToString());
        return false;
    }

    return true;
}