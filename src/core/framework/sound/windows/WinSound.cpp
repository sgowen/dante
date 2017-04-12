//
//  WinSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "WinSound.h"

WinSound::WinSound(int soundId, DirectX::SoundEffect& sound, float volume) : ISound(soundId),
m_isLooping(false),
m_isPaused(false)
{
    m_soundEffectInstance = sound.CreateInstance();
}

WinSound::~WinSound()
{
    m_soundEffectInstance->Pause();
    
    m_soundEffectInstance.reset();
}

void WinSound::play(bool isLooping)
{
    m_isLooping = isLooping;
    m_isPaused = false;
 
	m_soundEffectInstance->Stop();
    m_soundEffectInstance->Play(isLooping);
}

void WinSound::resume()
{
    if (m_isPaused)
    {
        m_soundEffectInstance->Resume();
        
        m_isPaused = false;
    }
}

void WinSound::pause()
{
    if (isPlaying())
    {
        m_soundEffectInstance->Pause();
        
        m_isPaused = true;
    }
}

void WinSound::stop()
{
    m_isLooping = false;
    m_isPaused = false;
    
    m_soundEffectInstance->Stop();
}

void WinSound::setVolume(float volume)
{
    m_soundEffectInstance->SetVolume(volume);
}

bool WinSound::isLooping()
{
    return m_isLooping;
}

bool WinSound::isPlaying()
{
	return m_soundEffectInstance->GetState() == DirectX::SoundState::PLAYING;
}

bool WinSound::isPaused()
{
    return m_isPaused;
}
