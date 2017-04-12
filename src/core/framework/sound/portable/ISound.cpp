//
//  ISound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "ISound.h"

ISound::ISound(int soundId) : m_iSoundId(soundId)
{
    // Empty
}

ISound::~ISound()
{
    // Empty
}

int ISound::getSoundId()
{
    return m_iSoundId;
}
