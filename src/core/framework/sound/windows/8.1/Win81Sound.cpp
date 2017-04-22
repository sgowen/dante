//
//  Win81Sound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Win81Sound.h"

#include "MediaEnginePlayer.h"
#include "Win81AudioEngineHelper.h"
#include "XAudio2SoundPlayer.h"

Win81Sound::Win81Sound(int soundId, int soundIndex, MediaEnginePlayer* mediaPlayer, float volume) : ISound(soundId),
m_mediaPlayer(mediaPlayer),
m_iSoundIndex(soundIndex),
m_isLooping(false),
m_isPaused(false)
{
    // Empty
}

Win81Sound::~Win81Sound()
{
    // Empty
}

void Win81Sound::play(bool isLooping)
{
    m_isLooping = isLooping;
    m_isPaused = false;
    
    if (m_mediaPlayer)
    {
        m_mediaPlayer->Play(m_isLooping);
    }
    else if (m_iSoundIndex > -1)
    {
		WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->PlaySound(m_iSoundIndex, m_isLooping);
    }
}

void Win81Sound::resume()
{
    if (m_isPaused)
    {
        if (m_mediaPlayer)
        {
            m_mediaPlayer->Play(m_isLooping);
        }
        else if (m_iSoundIndex > -1)
        {
			WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->PlaySound(m_iSoundIndex, m_isLooping);
        }
        
        m_isPaused = false;
    }
}

void Win81Sound::pause()
{
    if (isPlaying())
    {
        if (m_mediaPlayer)
        {
            m_mediaPlayer->Pause();
        }
        else if (m_iSoundIndex > -1)
        {
			WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->StopSound(m_iSoundIndex);
        }

		m_isPaused = true;
    }
}

void Win81Sound::stop()
{
    m_isLooping = false;
    m_isPaused = false;
    
	if (m_mediaPlayer)
	{
		m_mediaPlayer->Pause();
	}
	else if (m_iSoundIndex > -1)
	{
		WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->StopSound(m_iSoundIndex);
	}
}

void Win81Sound::setVolume(float volume)
{
    if (m_mediaPlayer)
    {
        m_mediaPlayer->SetVolume(volume);
    }
}

bool Win81Sound::isLooping()
{
    return m_isLooping;
}

bool Win81Sound::isPlaying()
{
    if (m_mediaPlayer)
    {
        return m_mediaPlayer->IsPlaying();
    }
    else if (m_iSoundIndex > -1)
    {
		return WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->IsSoundPlaying(m_iSoundIndex);
    }

	return false;
}

bool Win81Sound::isPaused()
{
    return m_isPaused;
}

void Win81Sound::setMediaEnginePlayer(MediaEnginePlayer* mediaPlayer)
{
	m_mediaPlayer = mediaPlayer;

	if (m_mediaPlayer && !m_isPaused)
	{
		play(m_isLooping);
	}
}