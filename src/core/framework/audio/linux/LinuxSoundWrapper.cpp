//
//  LinuxSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/audio/linux/LinuxSoundWrapper.h>

#include <framework/audio/linux/LinuxSound.h>

LinuxSoundWrapper::LinuxSoundWrapper(int soundId, const char *path, int numInstances) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new LinuxSound(soundId, path));
    }
}

LinuxSoundWrapper::~LinuxSoundWrapper()
{
    // Empty
}
