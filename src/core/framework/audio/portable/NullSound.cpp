//
//  NullSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/portable/NullSound.h"

NullSound::NullSound(int soundId, const char *path, float volume) : Sound(soundId),
_volume(volume),
_isLooping(false),
_isPaused(false),
_isMusic(soundId == 1337)
{
    // Empty
}

NullSound::~NullSound()
{
    // Empty
}

void NullSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
    
    // Empty
}

void NullSound::resume()
{
    if (_isPaused)
    {
        // Empty
        
        _isPaused = false;
    }
}

void NullSound::pause()
{
    if (isPlaying())
    {
        // Empty
        
        _isPaused = true;
    }
}

void NullSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    // Empty
}

void NullSound::setVolume(float volume)
{
    _volume = volume;
    
    // Empty
}

bool NullSound::isLooping()
{
    return _isLooping;
}

bool NullSound::isPlaying()
{
    return false;
}

bool NullSound::isPaused()
{
    return _isPaused;
}
