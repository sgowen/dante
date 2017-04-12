//
//  AndroidAssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAssetDataHandler__
#define __noctisgames__AndroidAssetDataHandler__

#define ANDROID_ASSETS (AndroidAssetDataHandler::getInstance())

#include "AssetDataHandler.h"

#include <jni.h>
#include <android/asset_manager_jni.h>

#include <string>

class AndroidAssetDataHandler : public AssetDataHandler
{
public:
	static AndroidAssetDataHandler* getInstance();
    
    void init(JNIEnv* jni, jobject activity);
    
    void deinit();
    
    const char* getPathInsideApk(const char* filePath);
    
    virtual FileData getAssetData(const char* relativePath);
    
    virtual void releaseAssetData(const FileData* fileData);

private:
    AAssetManager* mAssetManager;
    JavaVM* m_jvm;
    jstring m_javaApkFilesDirPath;
    const char* m_ApkFilesDirPath;
    std::string m_pathInsideApk;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAssetDataHandler();
    AndroidAssetDataHandler(const AndroidAssetDataHandler&);
    AndroidAssetDataHandler& operator=(const AndroidAssetDataHandler&);
};

#endif /* defined(__noctisgames__AndroidAssetDataHandler__) */
