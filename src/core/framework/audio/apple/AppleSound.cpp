//
//  AppleSound.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/apple/AppleSound.h>

#include <framework/audio/apple/ObjectALWrapper.h>

AppleSound::AppleSound(int soundId, const char *path, float volume) : Sound(soundId),
_objectALSoundIndex(-1),
_volume(volume),
_isLooping(false),
_isPaused(false),
_isMusic(soundId == 1337)
{
    if (_isMusic)
    {
        loadMusic(path);
    }
    else
    {
        _objectALSoundIndex = loadSound(path);
    }
}

AppleSound::~AppleSound()
{
    if (_isMusic)
    {
        stopMusic();
    }
    else
    {
        stopSound(_objectALSoundIndex);
    }
}

void AppleSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
    
    if (_isMusic)
    {
        stopMusic();
        playMusic(isLooping, _volume);
    }
    else
    {
        stopSound(_objectALSoundIndex);
        playSound(_objectALSoundIndex, _volume, _isLooping);
    }
}

void AppleSound::resume()
{
    if (_isPaused)
    {
        if (_isMusic)
        {
            resumeMusic();
        }
        else
        {
            resumeSound(_objectALSoundIndex);
        }
        
        _isPaused = false;
    }
}

void AppleSound::pause()
{
    if (isPlaying())
    {
        if (_isMusic)
        {
            pauseMusic();
        }
        else
        {
            pauseSound(_objectALSoundIndex);
        }
        
        _isPaused = true;
    }
}

void AppleSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    if (_isMusic)
    {
        stopMusic();
    }
    else
    {
        stopSound(_objectALSoundIndex);
    }
}

void AppleSound::setVolume(float volume)
{
    _volume = volume;
    
    if (_isMusic)
    {
        setMusicVolume(volume);
    }
    else
    {
        setSoundVolume(_objectALSoundIndex, volume);
    }
}

bool AppleSound::isLooping()
{
    return _isLooping;
}

bool AppleSound::isPlaying()
{
    if (_isMusic)
    {
        return isMusicPlaying();
    }
    else
    {
        return isSoundPlaying(_objectALSoundIndex);
    }
}

bool AppleSound::isPaused()
{
    return _isPaused;
}
