//
//  AndroidSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/android/AndroidSound.h"

#include "framework/audio/android/SoundService.hpp"
#include "framework/audio/android/OpenSLESSound.hpp"

AndroidSound::AndroidSound(SoundService* soundService, int soundId, const char *path, float volume) : Sound(soundId),
_soundService(soundService),
_sound(NULL),
_path(path),
_volume(volume),
_isLooping(false),
_isPaused(false),
_isMusic(soundId == 1337)
{
    _sound = _soundService->registerSound(path);
    _sound->load();
}

AndroidSound::~AndroidSound()
{
    // Empty
}

void AndroidSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
    
    _soundService->playSound(_sound);
}

void AndroidSound::resume()
{
    if (_isPaused)
    {
        // Empty
        
        _isPaused = false;
    }
}

void AndroidSound::pause()
{
    if (isPlaying())
    {
        // Empty
        
        _isPaused = true;
    }
}

void AndroidSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    // TODO
}

void AndroidSound::setVolume(float volume)
{
    _volume = volume;
    
    // Empty
}

bool AndroidSound::isLooping()
{
    return _isLooping;
}

bool AndroidSound::isPlaying()
{
    return false;
}

bool AndroidSound::isPaused()
{
    return _isPaused;
}
