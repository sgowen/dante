//
//  FileUtil.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/6/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/file/portable/FileUtil.h>

std::string FileUtil::filePathForConfig(const char* fileName)
{
    std::string ret;
#ifdef _WIN32
    ret = "assets\\config\\";
    ret += std::string(fileName);
#elif defined __linux__ && !defined(__ANDROID__)
    ret = "assets/config/";
    ret += std::string(fileName);
#else
    ret = fileName;
#endif
    
    return ret;
}

std::string FileUtil::filePathForMusic(const char* fileName)
{
    std::string ret;
#ifdef _WIN32
    ret = "assets\\music\\";
    ret += std::string(fileName);
#elif defined __linux__ && !defined(__ANDROID__)
    ret = "assets/music/";
    ret += std::string(fileName);
#else
    ret = fileName;
#endif
    
    return ret;
}

std::string FileUtil::filePathForShader(const char* fileName)
{
    std::string ret;
#ifdef _WIN32
    ret = "assets\\shaders\\";
    ret += std::string(fileName);
#elif defined __linux__ && !defined(__ANDROID__)
    ret = "assets/shaders/";
    ret += std::string(fileName);
#else
    ret = fileName;
#endif
    
    return ret;
}

std::string FileUtil::filePathForSound(const char* fileName)
{
    std::string ret;
#ifdef _WIN32
    ret = "assets\\sounds\\";
    ret += std::string(fileName);
#elif defined __linux__ && !defined(__ANDROID__)
    ret = "assets/sounds/";
    ret += std::string(fileName);
#else
    ret = fileName;
#endif
    
    return ret;
}

std::string FileUtil::filePathForTexture(const char* fileName)
{
    std::string ret;
#ifdef _WIN32
    ret = "assets\\textures\\";
    ret += std::string(fileName);
#elif defined __linux__ && !defined(__ANDROID__)
    ret = "assets/textures/";
    ret += std::string(fileName);
#else
    ret = fileName;
#endif
    
    return ret;
}
