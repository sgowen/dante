//
//  LinuxAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/Linux/linux/LinuxAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/linux/LinuxSoundWrapper.h"

#include <string>

LinuxAudioEngineHelper* LinuxAudioEngineHelper::getInstance()
{
    static LinuxAudioEngineHelper instance = LinuxAudioEngineHelper();
    return &instance;
}

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
    std::string s1(soundId == 1337 ? "data/music/" : "data/sounds/");
    s1 += std::string(path);
    std::string s2(".wav");
    s1 += s2;
    const char* finalPath = s1.c_str();

    LinuxSoundWrapper* sound = new LinuxSoundWrapper(soundId, finalPath, numInstances);

    return sound;
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
