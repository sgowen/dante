//
//  WinAudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "WinAudioEngineHelper.h"

#include "ISoundWrapper.h"

#include "WinSoundWrapper.h"

WinAudioEngineHelper* WinAudioEngineHelper::getInstance()
{
    static WinAudioEngineHelper instance = WinAudioEngineHelper();
    return &instance;
}

void WinAudioEngineHelper::update(int flags)
{
    if (flags == -1)
    {
        m_retryAudio = true;
        
        return;
    }
    
    if (m_retryAudio)
    {
        m_retryAudio = false;
        if (m_audEngine->Reset())
        {
            // TODO: restart any looped sounds here
        }
    }
    else if (!m_audEngine->Update())
    {
        if (m_audEngine->IsCriticalError())
        {
            m_retryAudio = true;
        }
    }
}

void WinAudioEngineHelper::pause()
{
    m_audEngine->Suspend();
}

void WinAudioEngineHelper::resume()
{
    m_audEngine->Resume();
}

ISoundWrapper* WinAudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    WinSoundWrapper* sound = new WinSoundWrapper(soundId, path, m_audEngine.get(), numInstances);
    
    return sound;
}

ISoundWrapper* WinAudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

WinAudioEngineHelper::WinAudioEngineHelper() : IAudioEngineHelper(), m_retryAudio(false)
{
	using namespace DirectX;

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    m_audEngine = std::make_unique<AudioEngine>(eflags);
}

WinAudioEngineHelper::~WinAudioEngineHelper()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
}
