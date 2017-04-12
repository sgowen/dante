//
//  AndroidAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "AndroidAssetDataHandler.h"

#include <assert.h>

AndroidAssetDataHandler* AndroidAssetDataHandler::getInstance()
{
    static AndroidAssetDataHandler instance = AndroidAssetDataHandler();
    
    return &instance;
}

void AndroidAssetDataHandler::init(JNIEnv* jni, jobject activity)
{
    jclass class_activity = jni->GetObjectClass(activity);
    jmethodID mid_getAssets = jni->GetMethodID(class_activity, "getAssets", "()Landroid/content/res/AssetManager;");
    assert(mid_getAssets != 0);
    
    jobject java_AssetManager = jni->CallObjectMethod(activity, mid_getAssets);
    
    mAssetManager = AAssetManager_fromJava(jni, java_AssetManager);
}

FileData AndroidAssetDataHandler::getAssetData(const char* relativePath)
{
    assert(relativePath != NULL);
    
    AAsset *asset = AAssetManager_open(mAssetManager, relativePath, AASSET_MODE_STREAMING);
    assert(asset != NULL);
    
    return (FileData)
    {
        AAsset_getLength(asset),
        AAsset_getBuffer(asset),
        asset
    };
}

void AndroidAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->file_handle != NULL);
    
    AAsset_close((AAsset*)fileData->file_handle);
}

AndroidAssetDataHandler::AndroidAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
