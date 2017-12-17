//
//  JsonFile.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/file/portable/JsonFile.h"

#include "framework/util/StringUtil.h"
#include "framework/util/macros.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#include "framework/file/apple/apple_asset_data_handler.h"
#endif

#if defined __ANDROID__
#include "framework/file/android/AndroidAssetDataHandler.h"
#endif

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

JsonFile::JsonFile(const char* filePath, bool useEncryption) : _filePath(filePath), _useEncryption(useEncryption)
{
    // Empty
}

void JsonFile::save()
{
    assert(_filePath);
    
    using namespace rapidjson;
    using namespace std;
    
    const char* finalPath = platformSpecificFilePath();
    FILE *file = openFile(finalPath, "w+");
    if (file)
    {
        static StringBuffer s;
        Writer<StringBuffer> w(s);
        
        s.Clear();
        
        w.StartObject();
        
        for (std::map<std::string, std::string>::iterator i = _keyValues.begin(); i != _keyValues.end(); ++i)
        {
            w.String((*i).first.c_str());
            w.String((*i).second.c_str());
        }
        
        w.EndObject();
        
        const char* data = s.GetString();
        
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
    
    using namespace rapidjson;
    using namespace std;
    
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
        
        rapidjson::Document d;
        d.Parse<0>(dataToRead.c_str());
        
        if (d.IsObject())
        {
            for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
            {
                _keyValues[i->name.GetString()] = i->value.GetString();
            }
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
