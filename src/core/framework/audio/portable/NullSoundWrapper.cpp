//
//  NullSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/portable/NullSoundWrapper.h"

#include "framework/audio/portable/NullSound.h"

NullSoundWrapper::NullSoundWrapper(int soundId, const char *path, int numInstances) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new NullSound(soundId, path));
    }
}

NullSoundWrapper::~NullSoundWrapper()
{
    // Empty
}
