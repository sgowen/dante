//
//  SaveData.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "SaveData.h"

#include "StringUtil.h"
#include "macros.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#include "apple_asset_data_handler.h"
#endif

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

SaveData* SaveData::getInstance()
{
    static SaveData instance = SaveData();
    return &instance;
}

void SaveData::config(const char* filePath)
{
    m_filePath = filePath;
}

void SaveData::save()
{
    assert(m_filePath);
    
    using namespace rapidjson;
    using namespace std;
    
    const char* finalPath;
#if TARGET_OS_IPHONE
    finalPath = getPathInsideNSDocuments(m_filePath);
#elif defined _WIN32
	#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
		finalPath = m_filePath;
	#else
		Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
		Platform::String^ ps_path = localFolder->Path;
		std::string s_path(ps_path->Begin(), ps_path->End());
		const char* path = s_path.c_str(); // std::string to const
		std::stringstream ss;
		ss << s_path << "\\" << m_filePath;

		finalPath = ss.str().c_str();
	#endif
#else
    finalPath = m_filePath;
#endif
    
    FILE *file;
#ifdef WIN32
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
        std::string encryptedData = StringUtil::encryptDecrypt(rawData);
        
        int sum = fprintf(file, "%s", encryptedData.c_str());
        
        UNUSED(sum);
        
        fclose(file);
    }
}

void SaveData::load()
{
    assert(m_filePath);
    
    using namespace rapidjson;
    using namespace std;
    
    const char* finalPath;
#if TARGET_OS_IPHONE
    finalPath = getPathInsideNSDocuments(m_filePath);
#elif defined _WIN32
	#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
		finalPath = m_filePath;
	#else
		Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
		Platform::String^ ps_path = localFolder->Path;
		std::string s_path(ps_path->Begin(), ps_path->End());
		const char* path = s_path.c_str(); // std::string to const
		std::stringstream ss;
		ss << s_path << "\\" << m_filePath;

		finalPath = ss.str().c_str();
	#endif
#else
    finalPath = m_filePath;
#endif
    
    FILE *file;
#ifdef WIN32
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
        
        std::string encryptedData;
        
        // allocate string space and set length
        encryptedData.resize(size);
        
        // go back to beginning of file for read
        rewind(file);
        
        // read 1*size bytes from sfile into ss
        fread(&encryptedData[0], 1, size, file);
        
        // close the file
        fclose(file);
        
        std::string rawData = StringUtil::encryptDecrypt(encryptedData);
        
        rapidjson::Document d;
        d.Parse<0>(rawData.c_str());
        
        if (d.IsObject())
        {
            for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
            {
                m_keyValues[i->name.GetString()] = i->value.GetString();
            }
        }
    }
}
    
void SaveData::clear()
{
    m_keyValues.clear();
    
    save();
}
    
std::string SaveData::findValue(std::string key)
{
    auto q = m_keyValues.find(key);
    
    if (q != m_keyValues.end())
    {
        return q->second;
    }
    
    return "";
}

std::map<std::string, std::string>& SaveData::getKeyValues()
{
    return m_keyValues;
}

SaveData::SaveData() : m_filePath(nullptr)
{
    // Empty
}
