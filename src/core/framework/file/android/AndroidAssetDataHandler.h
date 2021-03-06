//
//  AndroidAssetDataHandler.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAssetDataHandler__
#define __noctisgames__AndroidAssetDataHandler__

#include <framework/file/portable/AssetDataHandler.h>

#include <jni.h>
#include <android/asset_manager_jni.h>

#include <string>

class AndroidAssetDataHandler : public AssetDataHandler
{
public:
	static AndroidAssetDataHandler* getInstance();
    
    virtual FileData getAssetData(const char* relativePath);
    virtual void releaseAssetData(const FileData* fileData);
    
    void init(JNIEnv* jni, jobject activity);
    void deinit();
    const char* getPathInsideApk(const char* filePath);

private:
    AAssetManager* mAssetManager;
    JavaVM* _jvm;
    jstring _javaApkFilesDirPath;
    const char* _apkFilesDirPath;
    std::string _pathInsideApk;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAssetDataHandler();
    AndroidAssetDataHandler(const AndroidAssetDataHandler&);
    AndroidAssetDataHandler& operator=(const AndroidAssetDataHandler&);
};

#endif /* defined(__noctisgames__AndroidAssetDataHandler__) */
