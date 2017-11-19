//
//  SuperpoweredSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/superpowered/portable/SuperpoweredSoundWrapper.h"

#include "framework/audio/superpowered/portable/SuperpoweredSoundManager.h"
#include "framework/audio/superpowered/portable/SuperpoweredSound.h"

SuperpoweredSoundWrapper::SuperpoweredSoundWrapper(SuperpoweredSoundManager* manager, int soundId, const char *path, unsigned int sampleRate, int numInstances, int fileOffset, int fileLength) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new SuperpoweredSound(manager, soundId, path, sampleRate, fileOffset, fileLength));
    }
}

SuperpoweredSoundWrapper::~SuperpoweredSoundWrapper()
{
    // Empty
}
