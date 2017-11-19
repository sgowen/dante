//
//  SoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/portable/Sound.h"

#include "framework/util/NGSTDUtil.h"

SoundWrapper::SoundWrapper(int soundId, int numInstances) :
_soundIndex(0),
_soundId(soundId),
_numInstances(numInstances)
{
    // Empty
}

SoundWrapper::~SoundWrapper()
{
    NGSTDUtil::cleanUpVectorOfPointers(_sounds);
}

Sound* SoundWrapper::getSoundInstance()
{
    Sound* ret = _sounds[_soundIndex++];
    if (_soundIndex >= _numInstances)
    {
        _soundIndex = 0;
    }
    
    return ret;
}

std::vector<Sound *> SoundWrapper::getSounds()
{
    return _sounds;
}

int SoundWrapper::getSoundIndex()
{
    return _soundIndex;
}

int SoundWrapper::getSoundId()
{
    return _soundId;
}

int SoundWrapper::getNumInstances()
{
    return _numInstances;
}
