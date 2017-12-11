//
//  AndroidAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/android/AndroidAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/android/OpenSLESManager.h"
#include "framework/audio/android/OpenSLESSoundWrapper.h"
#include "framework/util/NGSTDUtil.h"

#include <assert.h>

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
    OpenSLESSoundWrapper* sound = new OpenSLESSoundWrapper(OpenSLESManager::getInstance(), soundId, path, numInstances);
    
    return sound;
}

SoundWrapper* AndroidAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

void AndroidAudioEngineHelper::init()
{
    OpenSLESManager::create();
}

void AndroidAudioEngineHelper::deinit()
{
    OpenSLESManager::destroy();
}

AndroidAudioEngineHelper::AndroidAudioEngineHelper() : AudioEngineHelper()
{
    // Empty
}

AndroidAudioEngineHelper::~AndroidAudioEngineHelper()
{
    // Empty
}
