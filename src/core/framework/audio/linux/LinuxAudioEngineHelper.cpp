//
//  LinuxAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/linux/LinuxAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/linux/LinuxSoundWrapper.h"

#include <AL/alut.h>

#include <cstddef>

void LinuxAudioEngineHelper::update(int flags)
{
    // Empty
}

void LinuxAudioEngineHelper::pause()
{
    // Empty
}

void LinuxAudioEngineHelper::resume()
{
    // Empty
}

SoundWrapper* LinuxAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    return new LinuxSoundWrapper(soundId, path, numInstances);
}

SoundWrapper* LinuxAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

LinuxAudioEngineHelper::LinuxAudioEngineHelper() : AudioEngineHelper()
{
    alutInit(NULL, NULL);
}

LinuxAudioEngineHelper::~LinuxAudioEngineHelper()
{
    alutExit();
}
