//
//  AndroidSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/android/AndroidSoundWrapper.h"

#include "framework/audio/android/SoundService.hpp"

#include "framework/audio/android/AndroidSound.h"

AndroidSoundWrapper::AndroidSoundWrapper(SoundService* soundService, int soundId, const char *path, int numInstances) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new AndroidSound(soundService, soundId, path));
    }
}

AndroidSoundWrapper::~AndroidSoundWrapper()
{
    // Empty
}
