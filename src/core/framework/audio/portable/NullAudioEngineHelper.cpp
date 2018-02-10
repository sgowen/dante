//
//  NullAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/portable/NullAudioEngineHelper.h>

#include <framework/audio/portable/SoundWrapper.h>

#include <framework/audio/portable/NullSoundWrapper.h>

void NullAudioEngineHelper::update(int flags)
{
    // Empty
}

void NullAudioEngineHelper::pause()
{
    // Empty
}

void NullAudioEngineHelper::resume()
{
    // Empty
}

SoundWrapper* NullAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    NullSoundWrapper* sound = new NullSoundWrapper(soundId, path, numInstances);
    
    return sound;
}

SoundWrapper* NullAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

NullAudioEngineHelper::NullAudioEngineHelper() : AudioEngineHelper()
{
    // Empty
}

NullAudioEngineHelper::~NullAudioEngineHelper()
{
    // Empty
}
