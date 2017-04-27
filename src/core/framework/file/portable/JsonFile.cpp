//
//  JsonFile.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "JsonFile.h"

#include "StringUtil.h"
#include "macros.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#include "apple_asset_data_handler.h"
#endif

#if defined __ANDROID__
#include "AndroidAssetDataHandler.h"
#endif

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

JsonFile::JsonFile(const char* filePath, bool useEncryption) : m_filePath(filePath), m_useEncryption(useEncryption)
{
    // Empty
}

void JsonFile::save()
{
    assert(m_filePath);
    
    using namespace rapidjson;
    using namespace std;
    
    const char* finalPath;
#if defined __ANDROID__
    finalPath = ANDROID_ASSETS->getPathInsideApk(m_filePath);
#elif TARGET_OS_IPHONE
    finalPath = getPathInsideNSDocuments(m_filePath);
#elif defined _WIN32
	#if !defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
		finalPath = m_filePath;
	#else
		Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
		Platform::String^ ps_path = localFolder->Path;
		std::string s_path(ps_path->Begin(), ps_path->End());
		const char* path = s_path.c_str(); // std::string to const
		std::stringstream ss;
		ss << path << "\\" << m_filePath;

		std::string finalPathS = ss.str();
		finalPath = finalPathS.c_str();
	#endif
#else
    finalPath = m_filePath;
#endif
    
    FILE *file;
#ifdef _WIN32
    errno_t err;
    if((err = fopen_s(&file, finalPath, "w+")) != 0)
    {
#else
    if ((file = fopen(finalPath, "w+")) == NULL)
    {
#endif
        return;
    }
    else
    {
        static StringBuffer s;
        Writer<StringBuffer> w(s);
        
        s.Clear();
        
        w.StartObject();
        
        for (std::map<std::string, std::string>::iterator i = m_keyValues.begin(); i != m_keyValues.end(); ++i)
        {
            w.String((*i).first.c_str());
            w.String((*i).second.c_str());
        }
        
        w.EndObject();
        
        const char* data = s.GetString();
        
        std::string rawData = std::string(data);
        std::string dataToWrite = m_useEncryption ? StringUtil::encryptDecrypt(rawData) : rawData;
        
        int sum = fprintf(file, "%s", dataToWrite.c_str());
        
        UNUSED(sum);
        
        fclose(file);
    }
}

void JsonFile::load()
{
    assert(m_filePath);
    
    using namespace rapidjson;
    using namespace std;
    
    const char* finalPath;

#if defined __ANDROID__
    finalPath = ANDROID_ASSETS->getPathInsideApk(m_filePath);
#elif TARGET_OS_IPHONE
    finalPath = getPathInsideNSDocuments(m_filePath);
#elif defined _WIN32
	#if !defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
		finalPath = m_filePath;
	#else
		Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
		Platform::String^ ps_path = localFolder->Path;
		std::string s_path(ps_path->Begin(), ps_path->End());
		const char* path = s_path.c_str(); // std::string to const
		std::stringstream ss;
		ss << path << "\\" << m_filePath;

		std::string finalPathS = ss.str();
		finalPath = finalPathS.c_str();
	#endif
#else
    finalPath = m_filePath;
#endif
    
    FILE *file;
#ifdef _WIN32
    errno_t err;
    if((err = fopen_s(&file, finalPath, "r")) != 0)
#else
    if ((file = fopen(finalPath, "r")) == NULL)
#endif
    {
        return;
    }
    else
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
        
        std::string dataToRead = m_useEncryption ? StringUtil::encryptDecrypt(rawData) : rawData;
        
        rapidjson::Document d;
        d.Parse<0>(dataToRead.c_str());
        
        if (d.IsObject())
        {
            for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
            {
                m_keyValues[i->name.GetString()] = i->value.GetString();
            }
        }
    }
}
    
void JsonFile::clear()
{
    m_keyValues.clear();
    
    save();
}
    
std::string JsonFile::findValue(std::string key)
{
    auto q = m_keyValues.find(key);
    
    if (q != m_keyValues.end())
    {
        return q->second;
    }
    
    return "";
}

void JsonFile::setValue(std::string key, std::string value)
{
    m_keyValues[key] = value;
}
