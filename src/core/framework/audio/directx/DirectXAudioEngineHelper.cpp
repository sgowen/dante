//
//  DirectXAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/directx/DirectXAudioEngineHelper.h>

#include <framework/audio/portable/SoundWrapper.h>

#include <framework/audio/directx/DirectXSoundWrapper.h>

void DirectXAudioEngineHelper::update(int flags)
{
    if (flags == -1)
    {
        _retryAudio = true;
        
        return;
    }
    
    if (_retryAudio)
    {
        _retryAudio = false;
        if (_audEngine->Reset())
        {
            // TODO, restart any looped sounds here
        }
    }
    else if (!_audEngine->Update())
    {
        if (_audEngine->IsCriticalError())
        {
            _retryAudio = true;
        }
    }
}

void DirectXAudioEngineHelper::pause()
{
    _audEngine->Suspend();
}

void DirectXAudioEngineHelper::resume()
{
    _audEngine->Resume();
}

SoundWrapper* DirectXAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    return new DirectXSoundWrapper(soundId, path, _audEngine.get(), numInstances);
}

SoundWrapper* DirectXAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

DirectXAudioEngineHelper::DirectXAudioEngineHelper() : AudioEngineHelper(), _retryAudio(false)
{
	using namespace DirectX;

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    _audEngine = std::make_unique<AudioEngine>(eflags);
}

DirectXAudioEngineHelper::~DirectXAudioEngineHelper()
{
    if (_audEngine)
    {
        _audEngine->Suspend();
    }
}
