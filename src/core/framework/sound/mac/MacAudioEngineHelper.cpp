//
//  MacAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MacAudioEngineHelper.h"

#include "ISoundWrapper.h"

#include "MacSoundWrapper.h"
#include "ObjectALWrapper.h"

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

ISoundWrapper* MacAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
	const char* bundlePath = getBundlePathForSoundWithName(path);
    
    MacSoundWrapper* sound = new MacSoundWrapper(soundId, bundlePath, numInstances);
    
    return sound;
}

ISoundWrapper* MacAudioEngineHelper::loadMusic(const char* path)
{
    const char* bundlePath = getBundlePathForSoundWithName(path);
    
    MacSoundWrapper* sound = new MacSoundWrapper(1337, bundlePath);
    
    return sound;
}

MacAudioEngineHelper::MacAudioEngineHelper() : IAudioEngineHelper()
{
    initObjectAL();
}

MacAudioEngineHelper::~MacAudioEngineHelper()
{
    deinitObjectAL();
}
