//
//  JsonFile.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/file/portable/JsonFile.h>

#include <framework/file/portable/AssetDataHandler.h>
#include <framework/file/portable/FileData.h>
#include <framework/util/StringUtil.h>
#include <framework/util/macros.h>
#include <framework/util/PlatformHelper.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#include <framework/file/apple/apple_asset_data_handler.h>
#endif

#if defined __ANDROID__
#include <framework/file/android/AndroidAssetDataHandler.h>
#endif

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

JsonFile::JsonFile(const char* filePath, bool isBundled, bool useEncryption) : _filePath(filePath), _isBundled(isBundled), _useEncryption(useEncryption), _deserializerFunc(NULL), _serializerFunc(NULL)
{
    // Empty
}

void JsonFile::setDeserializerFunc(DeserializerFunc deserializerFunc)
{
    _deserializerFunc = deserializerFunc;
}

void JsonFile::setSerializerFunc(SerializerFunc serializerFunc)
{
    _serializerFunc = serializerFunc;
}

void JsonFile::save()
{
    assert(_filePath);
    assert(!(_isBundled
             && (PlatformHelper::getPlatform() == NG_PLATFORM_ANDROID
                 || PlatformHelper::getPlatform() == NG_PLATFORM_IOS))); // Can't overwrite bundled files on Android or iOS

    const char* finalPath = platformSpecificFilePath();
    FILE *file = openFile(finalPath, "w+");
    if (file)
    {
        const char* data = serialize();

        std::string rawData = std::string(data);
        std::string dataToWrite = _useEncryption ? StringUtil::encryptDecrypt(rawData) : rawData;

        int sum = fprintf(file, "%s", dataToWrite.c_str());
        UNUSED(sum);

        fclose(file);
    }
}

void JsonFile::load()
{
    assert(_filePath);

    if (_isBundled)
    {
        const FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(_filePath);
        const char* data = (const char*)fileData.data;
        std::string rawData;
        rawData.clear();
        rawData = std::string(data);
        std::string dataToRead;
        dataToRead.clear();
        dataToRead = _useEncryption ? StringUtil::encryptDecrypt(rawData) : rawData;

        deserialize(dataToRead.c_str());

        AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fileData);
    }
    else
    {
        const char* finalPath = platformSpecificFilePath();

        FILE *file = openFile(finalPath, "r");
        if (file)
        {
            // seek to end of file
            fseek(file, 0, SEEK_END);

            // get current file position which is end from seek
            size_t size = ftell(file);

            std::string rawData;

            // allocate string space and set length
            rawData.resize(size);

            // go back to beginning of file for read
            rewind(file);

            // read 1*size bytes from sfile into ss
            fread(&rawData[0], 1, size, file);

            // close the file
            fclose(file);

            std::string dataToRead = _useEncryption ? StringUtil::encryptDecrypt(rawData) : rawData;

            deserialize(dataToRead.c_str());
        }
    }
}

void JsonFile::clear()
{
    _keyValues.clear();

    save();
}

std::string JsonFile::findValue(std::string key)
{
    auto q = _keyValues.find(key);

    if (q != _keyValues.end())
    {
        return q->second;
    }

    return "";
}

void JsonFile::setValue(std::string key, std::string value)
{
    _keyValues[key] = value;
}

const char* JsonFile::serialize()
{
    if (_serializerFunc)
    {
        return _serializerFunc();
    }
    else
    {
        using namespace rapidjson;

        static StringBuffer s;
        PrettyWriter<StringBuffer> w(s);

        s.Clear();

        w.StartObject();

        for (std::map<std::string, std::string>::iterator i = _keyValues.begin(); i != _keyValues.end(); ++i)
        {
            w.String((*i).first.c_str());
            w.String((*i).second.c_str());
        }

        w.EndObject();

        return s.GetString();
    }
}

void JsonFile::deserialize(const char *data)
{
    if (_deserializerFunc)
    {
        _deserializerFunc(data);
    }
    else
    {
        using namespace rapidjson;

        _keyValues.clear();

        rapidjson::Document d;
        d.Parse<kParseStopWhenDoneFlag>(data);

        if (d.IsObject())
        {
            for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
            {
                _keyValues[i->name.GetString()] = i->value.GetString();
            }
        }
    }
}

const char* JsonFile::platformSpecificFilePath()
{
    const char* ret;

#if defined __ANDROID__
    ret = AndroidAssetDataHandler::getInstance()->getPathInsideApk(_filePath);
#elif TARGET_OS_IPHONE
    ret = getPathInsideNSDocuments(_filePath);
#else
    ret = _filePath;
#endif

    return ret;
}

FILE* JsonFile::openFile(const char* path, const char* mode)
{
    FILE *file;

#ifdef _WIN32
    errno_t err;
    if ((err = fopen_s(&file, path, mode)) != 0)
    {
#else
    if ((file = fopen(path, mode)) == NULL)
    {
#endif
        return NULL;
    }

    return file;
}
