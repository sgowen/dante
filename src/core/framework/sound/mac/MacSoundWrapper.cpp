//
//  MacSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MacSoundWrapper.h"

#include "MacSound.h"

MacSoundWrapper::MacSoundWrapper(int soundId, const char *path, int numInstances) : ISoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new MacSound(soundId, path));
    }
}

MacSoundWrapper::~MacSoundWrapper()
{
    // Empty
}
