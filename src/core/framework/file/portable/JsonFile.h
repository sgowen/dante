//
//  JsonFile.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__JsonFile__
#define __noctisgames__JsonFile__

#include <map>
#include <string>

typedef void (*DeserializerFunc)(const char* data);
typedef const char* (*SerializerFunc)();

class JsonFile
{
public:
    static std::string filePathForConfigFile(const char* fileName);
    
    JsonFile(const char* filePath, bool isBundled = true, bool useEncryption = false);
    
    void setDeserializerFunc(DeserializerFunc deserializerFunc);
    void setSerializerFunc(SerializerFunc serializerFunc);
    void save();
    void load();
    void clear();
    std::string findValue(std::string key);
    void setValue(std::string key, std::string value);

private:
    const char* _filePath;
    bool _isBundled;
    bool _useEncryption;
    DeserializerFunc _deserializerFunc;
    SerializerFunc _serializerFunc;
    std::map<std::string, std::string> _keyValues;
    
    const char* serialize();
    void deserialize(const char* data);
    const char* platformSpecificFilePath();
    FILE* openFile(const char* path, const char* mode);
};

#endif /* defined(__noctisgames__JsonFile__) */
