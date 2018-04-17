//
//  OpenSLESSoundWrapper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/android/OpenSLESSoundWrapper.h>

#include <framework/audio/android/OpenSLESManager.h>

#include <framework/audio/android/OpenSLESSound.h>

OpenSLESSoundWrapper::OpenSLESSoundWrapper(OpenSLESManager* openSLESManager, int soundId, const char *path, int numInstances) : SoundWrapper(soundId, numInstances)
{
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new OpenSLESSound(openSLESManager, soundId, path));
    }
}

OpenSLESSoundWrapper::~OpenSLESSoundWrapper()
{
    // Empty
}
