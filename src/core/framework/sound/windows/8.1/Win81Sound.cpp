//
//  Win81Sound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Win81Sound.h"

#include "MediaEnginePlayer.h"

Win81Sound::Win81Sound(int soundId, MediaEnginePlayer* mediaPlayer, float volume) : ISound(soundId),
m_mediaPlayer(mediaPlayer),
m_isLooping(false),
m_isPaused(false)
{
    // Empty
}

Win81Sound::~Win81Sound()
{
    // TODO
}

void Win81Sound::play(bool isLooping)
{
    m_isLooping = isLooping;
    m_isPaused = false;
    
    // TODO
}

void Win81Sound::resume()
{
    if (m_isPaused)
    {
        // TODO
        
        m_isPaused = false;
    }
}

void Win81Sound::pause()
{
    if (isPlaying())
    {
        // TODO
        
        m_isPaused = true;
    }
}

void Win81Sound::stop()
{
    m_isLooping = false;
    m_isPaused = false;
    
    // TODO
}

void Win81Sound::setVolume(float volume)
{
    // TODO
}

bool Win81Sound::isLooping()
{
    return m_isLooping;
}

bool Win81Sound::isPlaying()
{
    // TODO
    return false;
}

bool Win81Sound::isPaused()
{
    return m_isPaused;
}
