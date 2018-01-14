//
//  DirectXAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/directx/DirectXAudioEngineHelper.h"

#include "framework/audio/portable/SoundWrapper.h"

#include "framework/audio/directx/DirectXSoundWrapper.h"

DirectXAudioEngineHelper* DirectXAudioEngineHelper::getInstance()
{
    static DirectXAudioEngineHelper instance = DirectXAudioEngineHelper();
    return &instance;
}

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
    std::string s(soundId == 1337 ? "assets\\music\\" : "assets\\sounds\\");
    s += std::string(path);
    const char* finalPath = s.c_str();

    DirectXSoundWrapper* sound = new DirectXSoundWrapper(soundId, finalPath, _audEngine.get(), numInstances);
    
    return sound;
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
