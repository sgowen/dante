//
//  ISoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "ISoundWrapper.h"

#include "ISound.h"

#include "NGSTDUtil.h"

ISoundWrapper::ISoundWrapper(int soundId, int numInstances) :
m_iSoundIndex(0),
m_iSoundId(soundId),
m_iNumInstances(numInstances)
{
    // Empty
}

ISoundWrapper::~ISoundWrapper()
{
    NGSTDUtil::cleanUpVectorOfPointers(m_sounds);
}

ISound* ISoundWrapper::getSoundInstance()
{
    ISound* ret = m_sounds[m_iSoundIndex++];
    if (m_iSoundIndex >= m_iNumInstances)
    {
        m_iSoundIndex = 0;
    }
    
    return ret;
}

std::vector<ISound *> ISoundWrapper::getSounds()
{
    return m_sounds;
}

int ISoundWrapper::getSoundIndex()
{
    return m_iSoundIndex;
}

int ISoundWrapper::getSoundId()
{
    return m_iSoundId;
}

int ISoundWrapper::getNumInstances()
{
    return m_iNumInstances;
}
