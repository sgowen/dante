//
//  WinSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "WinSound.h"

WinSound::WinSound(int soundId, DirectX::SoundEffect& sound, float volume) : ISound(soundId),
_isLooping(false),
_isPaused(false)
{
    _soundEffectInstance = sound.CreateInstance();
}

WinSound::~WinSound()
{
    _soundEffectInstance->Pause();
    
    _soundEffectInstance.reset();
}

void WinSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
 
	_soundEffectInstance->Stop();
    _soundEffectInstance->Play(isLooping);
}

void WinSound::resume()
{
    if (_isPaused)
    {
        _soundEffectInstance->Resume();
        
        _isPaused = false;
    }
}

void WinSound::pause()
{
    if (isPlaying())
    {
        _soundEffectInstance->Pause();
        
        _isPaused = true;
    }
}

void WinSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    _soundEffectInstance->Stop();
}

void WinSound::setVolume(float volume)
{
    _soundEffectInstance->SetVolume(volume);
}

bool WinSound::isLooping()
{
    return _isLooping;
}

bool WinSound::isPlaying()
{
	return _soundEffectInstance->GetState() == DirectX::SoundState::PLAYING;
}

bool WinSound::isPaused()
{
    return _isPaused;
}
