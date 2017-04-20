//
//  GameSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2016 Noctis Games. All rights reserved.
//

#include "pch.h"
#include "GameSound.h"
#include "SoundFileReader.h"

XAudio2SoundPlayer * GameSound::getSoundPlayerInstance()
{
	static XAudio2SoundPlayer *player = new XAudio2SoundPlayer(44100);
	return player;
}

GameSound::GameSound(Platform::String^ fileName)
{
	m_sounds[0] = loadSound(fileName);
	m_sounds[1] = loadSound(fileName);
	m_sounds[2] = loadSound(fileName);
	m_sounds[3] = loadSound(fileName);

	m_iSoundIndex = 0;
    m_isLooping = false;
}

void GameSound::play(bool isLoop)
{
	getSoundPlayerInstance()->PlaySound(m_sounds[m_iSoundIndex++], isLoop);

	if (m_iSoundIndex > 3)
	{
		m_iSoundIndex = 0;
	}
    
    m_isLooping = isLoop;
}

void GameSound::stop()
{
    getSoundPlayerInstance()->StopSound(m_sounds[0]);
	getSoundPlayerInstance()->StopSound(m_sounds[1]);
	getSoundPlayerInstance()->StopSound(m_sounds[2]);
	getSoundPlayerInstance()->StopSound(m_sounds[3]);
}

int GameSound::loadSound(Platform::String^ fileName)
{
	SoundFileReader sound(fileName);
	return getSoundPlayerInstance()->AddSound(sound.GetSoundFormat(), sound.GetSoundData());
}
