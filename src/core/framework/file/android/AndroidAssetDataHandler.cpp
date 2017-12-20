//
//  AndroidAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/file/android/AndroidAssetDataHandler.h"

#include <sstream>
#include <assert.h>

AndroidAssetDataHandler* AndroidAssetDataHandler::getInstance()
{
    static AndroidAssetDataHandler instance = AndroidAssetDataHandler();
    
    return &instance;
}

void AndroidAssetDataHandler::init(JNIEnv* jni, jobject activity)
{
    jni->GetJavaVM(&_jvm);
    
    jclass class_activity = jni->GetObjectClass(activity);
    jmethodID mid_getAssets = jni->GetMethodID(class_activity, "getAssets", "()Landroid/content/res/AssetManager;");
    assert(mid_getAssets != 0);
    
    jobject java_AssetManager = jni->CallObjectMethod(activity, mid_getAssets);
    
    mAssetManager = AAssetManager_fromJava(jni, java_AssetManager);
    
    jmethodID mid_getApplicationContext = jni->GetMethodID(class_activity, "getApplicationContext", "()Landroid/content/Context;");
    assert(mid_getApplicationContext != 0);
    
    jobject java_appContext = jni->CallObjectMethod(activity, mid_getApplicationContext);
    
    jclass class_appContext = jni->GetObjectClass(java_appContext);
    jmethodID mid_getFilesDir = jni->GetMethodID(class_appContext, "getFilesDir", "()Ljava/io/File;");
    assert(mid_getFilesDir != 0);
    
    jobject java_file = jni->CallObjectMethod(java_appContext, mid_getFilesDir);
    
    jclass class_file = jni->GetObjectClass(java_file);
    jmethodID mid_getAbsolutePath = jni->GetMethodID(class_file, "getAbsolutePath", "()Ljava/lang/String;");
    assert(mid_getAbsolutePath != 0);
    
    _javaApkFilesDirPath = (jstring) jni->CallObjectMethod(java_file, mid_getAbsolutePath);
    _apkFilesDirPath = jni->GetStringUTFChars(_javaApkFilesDirPath, JNI_FALSE);
}

void AndroidAssetDataHandler::deinit()
{
    JNIEnv* jni;
    
    jint status = _jvm->GetEnv((void**)&jni, JNI_VERSION_1_6);
    if (status != JNI_OK)
    {
        _jvm->AttachCurrentThread(&jni, NULL);
    }
    
    jni->ReleaseStringUTFChars(_javaApkFilesDirPath, _apkFilesDirPath);
    
    if (status != JNI_OK)
    {
        _jvm->DetachCurrentThread();
    }
}

const char* AndroidAssetDataHandler::getPathInsideApk(const char* filePath)
{
    std::stringstream ss;
    ss << _apkFilesDirPath << "/" << filePath;
    _pathInsideApk = ss.str();
    
    return _pathInsideApk.c_str();
}

FileData AndroidAssetDataHandler::getAssetData(const char* relativePath, bool isText)
{
    assert(relativePath != NULL);
    
    AAsset *asset = AAssetManager_open(mAssetManager, relativePath, AASSET_MODE_STREAMING);
    assert(asset != NULL);
    
    return FileData(AAsset_getLength(asset), const_cast<void*>(AAsset_getBuffer(asset)), asset);
}

void AndroidAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->file_handle != NULL);
    
    AAsset_close((AAsset*)fileData->file_handle);
}

AndroidAssetDataHandler::AndroidAssetDataHandler() : AssetDataHandler(), _jvm(NULL)
{
    // Empty
}
