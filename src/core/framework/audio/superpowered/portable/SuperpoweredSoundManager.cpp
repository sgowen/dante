//
//  SuperpoweredSoundManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/superpowered/portable/SuperpoweredSoundManager.h"

#include "framework/audio/superpowered/portable/SuperpoweredSound.h"
#include "framework/audio/superpowered/portable/SuperpoweredSoundWrapper.h"
#include "framework/util/NGSTDUtil.h"

#include <SuperpoweredSimple.h>

#import <stdlib.h>

SuperpoweredSoundManager::SuperpoweredSoundManager(unsigned int sampleRate, unsigned int bufferSize) :
_activeSounds(),
_sampleRate(sampleRate),
_soundIndex(1)
{
    for (int i = 0; i < MAX_NUM_SOUND_PLAYERS; ++i)
    {
        float* stereoBuffer;
#if defined __APPLE__
        if (posix_memalign((void **)&stereoBuffer, 16, 4096 + 128) != 0) abort(); // Allocating memory, aligned to 16.
#elif defined __ANDROID__
        stereoBuffer = (float *)memalign(16, (bufferSize + 16) * sizeof(float) * 2);
#endif
        _stereoBuffers.push_back(stereoBuffer);
        
        _activeSounds[i] = NULL;
    }
}

SuperpoweredSoundManager::~SuperpoweredSoundManager()
{
    for (int i = 0; i < MAX_NUM_SOUND_PLAYERS; ++i)
    {
        free(_stereoBuffers[i]);
    }
}

void SuperpoweredSoundManager::onSoundPlayed(SuperpoweredSound* sound)
{
    if (sound->getSoundId() == 1337)
    {
        // This is music
        _activeSounds[0] = sound;
        
        return;
    }
    
    for (int j = 1; j < MAX_NUM_SOUND_PLAYERS; ++j)
    {
        if (_activeSounds[j] == sound)
        {
            return;
        }
    }
    
    int count = 0;
    bool isGoodToBreak = false;
    while (true)
    {
        if (!_activeSounds[_soundIndex]
            || !_activeSounds[_soundIndex]->isPlaying())
        {
            _activeSounds[_soundIndex] = sound;
            
            isGoodToBreak = true;
        }
        
        _soundIndex++;
        if (_soundIndex >= MAX_NUM_SOUND_PLAYERS)
        {
            _soundIndex = 1;
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
        _activeSounds[0] = NULL;
        
        return;
    }
    
    for (int j = 1; j < MAX_NUM_SOUND_PLAYERS; ++j)
    {
        if (_activeSounds[j] == sound)
        {
            _activeSounds[j] = NULL;
            
            return;
        }
    }
}

bool SuperpoweredSoundManager::processMusic(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[0], _stereoBuffers[0], sampleRate);
}

bool SuperpoweredSoundManager::processSound1(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[1], _stereoBuffers[1], sampleRate);
}

bool SuperpoweredSoundManager::processSound2(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[2], _stereoBuffers[2], sampleRate);
}

bool SuperpoweredSoundManager::processSound3(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[3], _stereoBuffers[3], sampleRate);
}

bool SuperpoweredSoundManager::processSound4(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[4], _stereoBuffers[4], sampleRate);
}

bool SuperpoweredSoundManager::processSound5(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[5], _stereoBuffers[5], sampleRate);
}

bool SuperpoweredSoundManager::processSound6(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[6], _stereoBuffers[6], sampleRate);
}

bool SuperpoweredSoundManager::processSound7(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[7], _stereoBuffers[7], sampleRate);
}

bool SuperpoweredSoundManager::processSound8(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[8], _stereoBuffers[8], sampleRate);
}

bool SuperpoweredSoundManager::processSound9(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[9], _stereoBuffers[9], sampleRate);
}

bool SuperpoweredSoundManager::processSound10(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[10], _stereoBuffers[10], sampleRate);
}

bool SuperpoweredSoundManager::processSound11(void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    return processSound(output, numberOfSamples, _activeSounds[11], _stereoBuffers[11], sampleRate);
}

bool SuperpoweredSoundManager::processSound(void *output, unsigned int numberOfSamples, SuperpoweredSound *sound, float *stereoBuffer, unsigned int sampleRate)
{
    return sound && sound->process(stereoBuffer, output, numberOfSamples, sampleRate);
}
