//
//  AndroidAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/android/AndroidAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"
#include "framework/audio/android/SoundService.hpp"

#include "framework/audio/android/AndroidSoundWrapper.h"
#include "framework/util/NGSTDUtil.h"

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
    AndroidSoundWrapper* sound = new AndroidSoundWrapper(_soundService, soundId, path, numInstances);
    
    return sound;
}

SoundWrapper* AndroidAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

void AndroidAudioEngineHelper::init(JNIEnv* jni, jobject activity)
{
    jclass class_activity = jni->GetObjectClass(activity);
    
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
            sampleRate = _sampleRate;
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
            bufferSize = _bufferSize;
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
    _bufferSize = bufferSize;
    
    /// TODO, use _sampleRate and _bufferSize

    assert(_soundService->start() == STATUS_OK);
}

void AndroidAudioEngineHelper::deinit()
{
    _soundService->stop();
}

AndroidAudioEngineHelper::AndroidAudioEngineHelper() : AudioEngineHelper(), _sampleRate(44100), _bufferSize(512), _soundService(new SoundService())
{
    // Empty
}

AndroidAudioEngineHelper::~AndroidAudioEngineHelper()
{
    delete _soundService;
}
