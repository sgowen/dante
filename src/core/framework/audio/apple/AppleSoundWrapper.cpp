//
//  AppleSoundWrapper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/audio/apple/AppleSoundWrapper.h>

#include <framework/audio/apple/AppleSound.h>

AppleSoundWrapper::AppleSoundWrapper(int soundId, const char *path, int numInstances) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new AppleSound(soundId, path));
    }
}

AppleSoundWrapper::~AppleSoundWrapper()
{
    // Empty
}
