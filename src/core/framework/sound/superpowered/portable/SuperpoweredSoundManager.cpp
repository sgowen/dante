//
//  SuperpoweredSoundManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "SuperpoweredSoundManager.h"

#include "SuperpoweredSound.h"
#include "SuperpoweredSoundWrapper.h"
#include "NGSTDUtil.h"

#include <SuperpoweredSimple.h>

#import <stdlib.h>

SuperpoweredSoundManager::SuperpoweredSoundManager(unsigned int sampleRate, unsigned int bufferSize) :
m_activeSounds(),
m_iSampleRate(sampleRate),
m_iSoundIndex(1)
{
    for (int i = 0; i < MAX_NUM_SOUND_PLAYERS; ++i)
    {
        float* stereoBuffer;
#if defined __APPLE__
        if (posix_memalign((void **)&stereoBuffer, 16, 4096 + 128) != 0) abort(); // Allocating memory, aligned to 16.
#elif defined __ANDROID__
        stereoBuffer = (float *)memalign(16, (bufferSize + 16) * sizeof(float) * 2);
#endif
        m_stereoBuffers.push_back(stereoBuffer);
        
        m_activeSounds[i] = nullptr;
    }
}

SuperpoweredSoundManager::~SuperpoweredSoundManager()
{
    for (int i = 0; i < MAX_NUM_SOUND_PLAYERS; ++i)
    {
        free(m_stereoBuffers[i]);
    }
}

void SuperpoweredSoundManager::onSoundPlayed(SuperpoweredSound* sound)
{
    if (sound->getSoundId() == 1337)
    {
        // This is music
        m_activeSounds[0] = sound;
        
        return;
    }
    
    for (int j = 1; j < MAX_NUM_SOUND_PLAYERS; ++j)
    {
        if (m_activeSounds[j] == sound)
        {
            return;
        }
    }
    
    int count = 0;
    bool isGoodToBreak = false;
    while (true)
    {
        if (!m_activeSounds[m_iSoundIndex]
            || !m_activeSounds[m_iSoundIndex]->isPlaying())
        {
            m_activeSounds[m_iSoundIndex] = sound;
            
            isGoodToBreak = true;
        }
        
        m_iSoundIndex++;
        if (m_iSoundIndex >= MAX_NUM_SOUND_PLAYERS)
        {
            m_iSoundIndex = 1;
        }
        
        count++;
        if (isGoodToBreak
            || count >= (MAX_NUM_SOUND_PLAYERS - 1))
        {
            break;
        }
    }
}

void SuperpoweredSoundManager::onSoundStopped(SuperpoweredSound* sound)
{
    if (sound->getSoundId() == 1337)
    {
        // This is music
        m_activeSounds[0] = nullptr;
        
        return;
    }
    
    for (int j = 1; j < MAX_NUM_SOUND_PLAYERS; ++j)
    {
        if (m_activeSounds[j] == sound)
        {
            m_activeSounds[j] = nullptr;
            
            return;
        }
    }
}

bool SuperpoweredSoundManager::processMusic(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[0], m_stereoBuffers[0], sampleRate);
}

bool SuperpoweredSoundManager::processSound1(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[1], m_stereoBuffers[1], sampleRate);
}

bool SuperpoweredSoundManager::processSound2(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[2], m_stereoBuffers[2], sampleRate);
}

bool SuperpoweredSoundManager::processSound3(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[3], m_stereoBuffers[3], sampleRate);
}

bool SuperpoweredSoundManager::processSound4(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[4], m_stereoBuffers[4], sampleRate);
}

bool SuperpoweredSoundManager::processSound5(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[5], m_stereoBuffers[5], sampleRate);
}

bool SuperpoweredSoundManager::processSound6(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[6], m_stereoBuffers[6], sampleRate);
}

bool SuperpoweredSoundManager::processSound7(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[7], m_stereoBuffers[7], sampleRate);
}

bool SuperpoweredSoundManager::processSound8(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[8], m_stereoBuffers[8], sampleRate);
}

bool SuperpoweredSoundManager::processSound9(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[9], m_stereoBuffers[9], sampleRate);
}

bool SuperpoweredSoundManager::processSound10(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[10], m_stereoBuffers[10], sampleRate);
}

bool SuperpoweredSoundManager::processSound11(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, m_activeSounds[11], m_stereoBuffers[11], sampleRate);
}

bool SuperpoweredSoundManager::processSound(void *output, unsigned int numberOfSamples, SuperpoweredSound *sound, float *stereoBuffer, unsigned int sampleRate)
{
    return sound && sound->process(stereoBuffer, output, numberOfSamples, sampleRate);
}
