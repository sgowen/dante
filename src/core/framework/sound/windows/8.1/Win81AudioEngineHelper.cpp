//
//  Win81AudioEngineHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Win81AudioEngineHelper.h"

#include "ISoundWrapper.h"
#include "MediaEnginePlayer.h"
#include "Win81SoundWrapper.h"
#include "Win81Sound.h"

#include "Direct3DManager.h"

XAudio2SoundPlayer * Win81AudioEngineHelper::getSoundPlayerInstance()
{
    static XAudio2SoundPlayer player = XAudio2SoundPlayer(44100);
    return &player;
}

Win81AudioEngineHelper* Win81AudioEngineHelper::getInstance()
{
    static Win81AudioEngineHelper instance = Win81AudioEngineHelper();
    return &instance;
}

void Win81AudioEngineHelper::update(int flags)
{
    // Check flags to determine whether or not to restart MediaEnginePlayer
    
    if (flags == 1
        && !m_mediaPlayer)
    {
        // Load Media Player
		ID3D11Device* d3dDevice = Direct3DManager::getD3dDevice();
        m_mediaPlayer = std::unique_ptr<MediaEnginePlayer>(new MediaEnginePlayer);
        m_mediaPlayer->Initialize(d3dDevice, DXGI_FORMAT_B8G8R8A8_UNORM);

		if (m_music)
		{
			ISound* sound = m_music->getSoundInstance();
			((Win81Sound*)sound)->setMediaEnginePlayer(m_mediaPlayer.get());
		}
    }
    else if (flags == -1
             && m_mediaPlayer)
    {
        m_mediaPlayer->Shutdown();
        m_mediaPlayer = nullptr;

		if (m_music)
		{
			ISound* sound = m_music->getSoundInstance();
			((Win81Sound*)sound)->setMediaEnginePlayer(nullptr);
		}
    }
}

void Win81AudioEngineHelper::pause()
{
    getSoundPlayerInstance()->Suspend();
}

void Win81AudioEngineHelper::resume()
{
    getSoundPlayerInstance()->Resume();
}

ISoundWrapper* Win81AudioEngineHelper::loadSound(int soundId, const char *path, int numInstances)
{
    Win81SoundWrapper* sound = new Win81SoundWrapper(soundId, path, numInstances, soundId == 1337 ? m_mediaPlayer.get() : nullptr);
    
	if (soundId == 1337)
	{
		m_music = sound;
	}

    return sound;
}

ISoundWrapper* Win81AudioEngineHelper::loadMusic(const char* path)
{
    return loadSound(1337, path);
}

Win81AudioEngineHelper::Win81AudioEngineHelper() : IAudioEngineHelper(), m_mediaPlayer(nullptr), m_music(nullptr)
{
    // Empty
}

Win81AudioEngineHelper::~Win81AudioEngineHelper()
{
    if (m_mediaPlayer)
    {
        m_mediaPlayer->Shutdown();
        m_mediaPlayer = nullptr;
    }
}
