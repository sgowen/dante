//
//  FileUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/6/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FileUtil__
#define __noctisgames__FileUtil__

#include <string>

class FileUtil
{
public:
    static std::string filePathForConfig(const char* fileName);
    static std::string filePathForMusic(const char* fileName);
    static std::string filePathForShader(const char* fileName);
    static std::string filePathForSound(const char* fileName);
    static std::string filePathForTexture(const char* fileName);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    FileUtil();
    ~FileUtil();
    FileUtil(const FileUtil&);
    FileUtil& operator=(const FileUtil&);
};

#endif /* defined(__noctisgames__FileUtil__) */
