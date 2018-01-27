//
//  AppleAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/apple/AppleAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/apple/AppleSoundWrapper.h"
#include "framework/audio/apple/ObjectALWrapper.h"

#include "framework/file/apple/apple_asset_data_handler.h"

void AppleAudioEngineHelper::update(int flags)
{
    // Empty
}

void AppleAudioEngineHelper::pause()
{
    pauseObjectAL();
}

void AppleAudioEngineHelper::resume()
{
    resumeObjectAL();
}

SoundWrapper* AppleAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
	const char* bundlePath = getBundlePath(path);
    
    AppleSoundWrapper* sound = new AppleSoundWrapper(soundId, bundlePath, numInstances);
    
    return sound;
}

SoundWrapper* AppleAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

AppleAudioEngineHelper::AppleAudioEngineHelper() : AudioEngineHelper()
{
    initObjectAL();
}

AppleAudioEngineHelper::~AppleAudioEngineHelper()
{
    deinitObjectAL();
}
