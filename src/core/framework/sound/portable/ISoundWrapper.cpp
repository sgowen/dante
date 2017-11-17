//
//  ISoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ISoundWrapper.h"

#include "ISound.h"

#include "NGSTDUtil.h"

ISoundWrapper::ISoundWrapper(int soundId, int numInstances) :
_soundIndex(0),
_soundId(soundId),
_numInstances(numInstances)
{
    // Empty
}

ISoundWrapper::~ISoundWrapper()
{
    NGSTDUtil::cleanUpVectorOfPointers(_sounds);
}

ISound* ISoundWrapper::getSoundInstance()
{
    ISound* ret = _sounds[_soundIndex++];
    if (_soundIndex >= _numInstances)
    {
        _soundIndex = 0;
    }
    
    return ret;
}

std::vector<ISound *> ISoundWrapper::getSounds()
{
    return _sounds;
}

int ISoundWrapper::getSoundIndex()
{
    return _soundIndex;
}

int ISoundWrapper::getSoundId()
{
    return _soundId;
}

int ISoundWrapper::getNumInstances()
{
    return _numInstances;
}
