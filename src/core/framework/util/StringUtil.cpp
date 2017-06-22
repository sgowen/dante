//
//  StringUtil.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/1/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "StringUtil.h"

#include <stdarg.h>

std::string StringUtil::encryptDecrypt(std::string input)
{
    char key[3] = {'N', 'G', 'S'}; // Any chars will work, in an array of any size
    std::string output = input;
    
    for (unsigned int i = 0; i < input.size(); ++i)
    {
        output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];
    }
    
    return output;
}

void StringUtil::encryptDecrypt(unsigned char* input, unsigned char* output, const long dataLength)
{
    char key[3] = {'N', 'G', 'S'}; // Any chars will work, in an array of any size
    
    for (unsigned int i = 0; i < dataLength; ++i)
    {
        output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];
    }
}

#ifndef _WIN32
void OutputDebugStringA(const char* inString)
{
    printf("%s", inString);
}
#endif

std::string StringUtil::format(const char* inFormat, ...)
{
    char temp[4096];
    
    va_list args;
    va_start (args, inFormat);
    
#if _WIN32
    _vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
    vsnprintf(temp, 4096, inFormat, args);
#endif
    return std::string(temp);
}

void StringUtil::log(const char* inFormat, ...)
{
    char temp[4096];
    
    va_list args;
    va_start (args, inFormat);
    
#if _WIN32
    _vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
    vsnprintf(temp, 4096, inFormat, args);
#endif
    
    OutputDebugStringA(temp);
    OutputDebugStringA("\n");
}
