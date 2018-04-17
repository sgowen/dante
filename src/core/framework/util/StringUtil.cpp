//
//  StringUtil.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/1/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/StringUtil.h>

#include <stdarg.h>

#if defined __ANDROID__
#include <android/log.h>
#endif

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
#if defined __ANDROID__
    __android_log_print(ANDROID_LOG_DEBUG, "NoctisGames", "%s", inString);
#else
    printf("%s", inString);
#endif
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
#elif defined __ANDROID__
    __android_log_vprint(ANDROID_LOG_DEBUG, "NoctisGames", inFormat, args);
#else
    vsnprintf(temp, 4096, inFormat, args);
#endif
    
    OutputDebugStringA(temp);
    OutputDebugStringA("\n");
}
