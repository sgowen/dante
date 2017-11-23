//
//  AndroidAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/android/AndroidAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include <assert.h>
#include <sstream>

AndroidAudioEngineHelper* AndroidAudioEngineHelper::getInstance()
{
    static AndroidAudioEngineHelper instance = AndroidAudioEngineHelper();
    return &instance;
}

void AndroidAudioEngineHelper::update(int flags)
{
    // Empty
}

void AndroidAudioEngineHelper::pause()
{
    // Empty
}

void AndroidAudioEngineHelper::resume()
{
    // Empty
}

SoundWrapper* AndroidAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    JNIEnv* jni;
    
    jint status = _jvm->GetEnv((void**)&jni, JNI_VERSION_1_6);
    if (status != JNI_OK)
    {
        _jvm->AttachCurrentThread(&jni, NULL);
    }
    
    jclass class_resources = jni->GetObjectClass(_resources);
    
    jmethodID mid_getIdentifier = jni->GetMethodID(class_resources, "getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
    assert(mid_getIdentifier != 0);
    
    jstring java_soundName = jni->NewStringUTF(path);
    jstring java_defType = jni->NewStringUTF("raw");
    
    jint resId = jni->CallIntMethod(_resources, mid_getIdentifier, java_soundName, java_defType, _packageName);
    
    jni->DeleteLocalRef(java_soundName);
    jni->DeleteLocalRef(java_defType);
    
    jmethodID mid_openRawResourceFd = jni->GetMethodID(class_resources, "openRawResourceFd", "(I)Landroid/content/res/AssetFileDescriptor;");
    assert(mid_openRawResourceFd != 0);
    
    jobject java_assetFileDescriptor = jni->CallObjectMethod(_resources, mid_openRawResourceFd, resId);
    
    jclass class_assetFileDescriptor = jni->GetObjectClass(java_assetFileDescriptor);
    
    jmethodID mid_getStartOffset = jni->GetMethodID(class_assetFileDescriptor, "getStartOffset", "()J");
    assert(mid_getStartOffset != 0);
    
    jlong java_fileOffset = jni->CallLongMethod(java_assetFileDescriptor, mid_getStartOffset);
    
    jmethodID mid_getLength = jni->GetMethodID(class_assetFileDescriptor, "getLength", "()J");
    assert(mid_getLength != 0);
    
    jlong java_fileLength = jni->CallLongMethod(java_assetFileDescriptor, mid_getLength);
    
    int fileOffset = (int) java_fileOffset;
    int fileLength = (int) java_fileLength;
    
    jmethodID mid_getParcelFileDescriptor = jni->GetMethodID(class_assetFileDescriptor, "getParcelFileDescriptor", "()Landroid/os/ParcelFileDescriptor;");
    assert(mid_getParcelFileDescriptor != 0);
    
    jobject java_parcelFileDescriptor = jni->CallObjectMethod(java_assetFileDescriptor, mid_getParcelFileDescriptor);
    
    jclass class_parcelFileDescriptor = jni->GetObjectClass(java_parcelFileDescriptor);
    jmethodID mid_close = jni->GetMethodID(class_parcelFileDescriptor, "close", "()V");
    assert(mid_close != 0);
    
    jni->CallVoidMethod(java_parcelFileDescriptor, mid_close);
    
    jni->DeleteLocalRef(java_assetFileDescriptor);
    jni->DeleteLocalRef(java_parcelFileDescriptor);
    
    if (status != JNI_OK)
    {
        _jvm->DetachCurrentThread();
    }
    
    /// TODO
    
    return NULL;
}

SoundWrapper* AndroidAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

void AndroidAudioEngineHelper::init(JNIEnv* jni, jobject activity)
{
    jni->GetJavaVM(&_jvm);
    
    jclass class_activity = jni->GetObjectClass(activity);
    
    jmethodID mid_getPackageResourcePath = jni->GetMethodID(class_activity, "getPackageResourcePath", "()Ljava/lang/String;");
    assert(mid_getPackageResourcePath != 0);
    _javaPackageResourcePath = (jstring) jni->CallObjectMethod(activity, mid_getPackageResourcePath);
    _packageResourcePath = jni->GetStringUTFChars(_javaPackageResourcePath, JNI_FALSE);
    
    jmethodID mid_getPackageName = jni->GetMethodID(class_activity, "getPackageName", "()Ljava/lang/String;");
    assert(mid_getPackageName != 0);
    jobject java_PackageName = jni->CallObjectMethod(activity, mid_getPackageName);
    _packageName = (jstring) jni->NewGlobalRef(java_PackageName);
    
    jmethodID mid_getResources = jni->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
    assert(mid_getResources != 0);
    jobject java_resources = jni->CallObjectMethod(activity, mid_getResources);
    _resources = jni->NewGlobalRef(java_resources);
    
    // Get the device's sample rate and buffer size to enable low-latency Android audio output, if available.
    jmethodID mid_getSystemService = jni->GetMethodID(class_activity, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    assert(mid_getSystemService != 0);
    
    jstring java_audio = jni->NewStringUTF("audio");
    jobject java_audioManager = jni->CallObjectMethod(activity, mid_getSystemService, java_audio);
    
    jni->DeleteLocalRef(java_audio);
    
    int sampleRate;
    int bufferSize;
    
    jclass class_audioManager = jni->GetObjectClass(java_audioManager);
    
    {
        jmethodID mid_getProperty = jni->GetMethodID(class_audioManager, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
        assert(mid_getProperty != 0);
        
        jstring java_property = jni->NewStringUTF("android.media.property.OUTPUT_SAMPLE_RATE");
        jstring java_sampleRateString = (jstring) jni->CallObjectMethod(java_audioManager, mid_getProperty, java_property);
        if (java_sampleRateString == NULL)
        {
            sampleRate = 44100;
        }
        else
        {
            const char *sampleRateString = jni->GetStringUTFChars(java_sampleRateString, JNI_FALSE);
            
            std::stringstream ss;
            ss << sampleRateString;
            
            ss >> sampleRate;
        }
        
        jni->DeleteLocalRef(java_property);
        jni->DeleteLocalRef(java_sampleRateString);
    }
    
    {
        jmethodID mid_getProperty = jni->GetMethodID(class_audioManager, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
        assert(mid_getProperty != 0);
        
        jstring java_property = jni->NewStringUTF("android.media.property.OUTPUT_FRAMES_PER_BUFFER");
        jstring java_bufferSizeString = (jstring) jni->CallObjectMethod(java_audioManager, mid_getProperty, java_property);
        if (java_bufferSizeString == NULL)
        {
            bufferSize = 512;
        }
        else
        {
            const char *bufferSizeString = jni->GetStringUTFChars(java_bufferSizeString, JNI_FALSE);
            
            std::stringstream ss;
            ss << bufferSizeString;
            
            ss >> bufferSize;
        }
        
        jni->DeleteLocalRef(java_property);
        jni->DeleteLocalRef(java_bufferSizeString);
    }
    
    jni->DeleteLocalRef(java_audioManager);
    
    _sampleRate = sampleRate;
}

void AndroidAudioEngineHelper::deinit()
{
    JNIEnv* jni;
    
    jint status = _jvm->GetEnv((void**)&jni, JNI_VERSION_1_6);
    if (status != JNI_OK)
    {
        _jvm->AttachCurrentThread(&jni, NULL);
    }
    
    jni->DeleteLocalRef(_resources);
    jni->DeleteLocalRef(_packageName);
    jni->ReleaseStringUTFChars(_javaPackageResourcePath, _packageResourcePath);
    
    if (status != JNI_OK)
    {
        _jvm->DetachCurrentThread();
    }
}

AndroidAudioEngineHelper::AndroidAudioEngineHelper() : AudioEngineHelper(), _sampleRate(44100)
{
    // Empty
}
