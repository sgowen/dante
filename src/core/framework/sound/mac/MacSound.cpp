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
m_iObjectALSoundIndex(-1),
m_fVolume(volume),
m_isLooping(false),
m_isPaused(false),
m_isMusic(soundId == 1337)
{
    if (m_isMusic)
    {
        loadMusic(path);
    }
    else
    {
        m_iObjectALSoundIndex = loadSound(path);
    }
}

MacSound::~MacSound()
{
    if (m_isMusic)
    {
        stopMusic();
    }
    else
    {
        stopSound(m_iObjectALSoundIndex);
    }
}

void MacSound::play(bool isLooping)
{
    m_isLooping = isLooping;
    m_isPaused = false;
    
    if (m_isMusic)
    {
        stopMusic();
        playMusic(isLooping, m_fVolume);
    }
    else
    {
        stopSound(m_iObjectALSoundIndex);
        playSound(m_iObjectALSoundIndex, m_fVolume, m_isLooping);
    }
}

void MacSound::resume()
{
    if (m_isPaused)
    {
        if (m_isMusic)
        {
            resumeMusic();
        }
        else
        {
            resumeSound(m_iObjectALSoundIndex);
        }
        
        m_isPaused = false;
    }
}

void MacSound::pause()
{
    if (isPlaying())
    {
        if (m_isMusic)
        {
            pauseMusic();
        }
        else
        {
            pauseSound(m_iObjectALSoundIndex);
        }
        
        m_isPaused = true;
    }
}

void MacSound::stop()
{
    m_isLooping = false;
    m_isPaused = false;
    
    if (m_isMusic)
    {
        stopMusic();
    }
    else
    {
        stopSound(m_iObjectALSoundIndex);
    }
}

void MacSound::setVolume(float volume)
{
    if (m_isMusic)
    {
        setMusicVolume(volume);
    }
    else
    {
        setSoundVolume(m_iObjectALSoundIndex, volume);
    }
}

bool MacSound::isLooping()
{
    return m_isLooping;
}

bool MacSound::isPlaying()
{
    if (m_isMusic)
    {
        return isMusicPlaying();
    }
    else
    {
        return isSoundPlaying(m_iObjectALSoundIndex);
    }
}

bool MacSound::isPaused()
{
    return m_isPaused;
}
