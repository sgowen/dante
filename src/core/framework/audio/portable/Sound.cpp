//
//  Sound.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/portable/Sound.h>

Sound::Sound(int soundId) : _soundId(soundId)
{
    // Empty
}

Sound::~Sound()
{
    // Empty
}

int Sound::getSoundId()
{
    return _soundId;
}
