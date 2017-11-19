//
//  MacAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/mac/MacAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/mac/MacSoundWrapper.h"
#include "framework/audio/mac/ObjectALWrapper.h"

MacAudioEngineHelper* MacAudioEngineHelper::getInstance()
{
    static MacAudioEngineHelper instance = MacAudioEngineHelper();
    return &instance;
}

void MacAudioEngineHelper::update(int flags)
{
    // Empty
}

void MacAudioEngineHelper::pause()
{
    pauseObjectAL();
}

void MacAudioEngineHelper::resume()
{
    resumeObjectAL();
}

SoundWrapper* MacAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
	const char* bundlePath = getBundlePathForSoundWithName(path);
    
    MacSoundWrapper* sound = new MacSoundWrapper(soundId, bundlePath, numInstances);
    
    return sound;
}

SoundWrapper* MacAudioEngineHelper::loadMusic(const char* path)
{
    const char* bundlePath = getBundlePathForSoundWithName(path);
    
    MacSoundWrapper* sound = new MacSoundWrapper(1337, bundlePath);
    
    return sound;
}

MacAudioEngineHelper::MacAudioEngineHelper() : AudioEngineHelper()
{
    initObjectAL();
}

MacAudioEngineHelper::~MacAudioEngineHelper()
{
    deinitObjectAL();
}
