//
//  MacSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MacSound.h"

#include "ObjectALWrapper.h"

MacSound::MacSound(int soundId, const char *path, float volume) : ISound(soundId),
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

MacSound::~MacSound()
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

void MacSound::play(bool isLooping)
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

void MacSound::resume()
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

void MacSound::pause()
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

void MacSound::stop()
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

void MacSound::setVolume(float volume)
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

bool MacSound::isLooping()
{
    return _isLooping;
}

bool MacSound::isPlaying()
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

bool MacSound::isPaused()
{
    return _isPaused;
}
