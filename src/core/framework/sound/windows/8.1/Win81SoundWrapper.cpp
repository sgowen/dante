//
//  Win81SoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Win81SoundWrapper.h"

#include "MediaEnginePlayer.h"

#include "Win81Sound.h"
#include "Win81AudioEngineHelper.h"
#include "XAudio2SoundPlayer.h"
#include "SoundFileReader.h"

Win81SoundWrapper::Win81SoundWrapper(int soundId, const char *path, int numInstances, MediaEnginePlayer* mediaPlayer) : ISoundWrapper(soundId, numInstances)
{
    size_t len = strlen(path);
    
    char* wavFileName = new char[len + 5];
    
	strcpy_s(wavFileName, len + 5, path);
    wavFileName[len] = '.';
    wavFileName[len + 1] = 'w';
    wavFileName[len + 2] = 'a';
    wavFileName[len + 3] = 'v';
    wavFileName[len + 4] = '\0';
    
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, wavFileName, -1, wString, 4096);
    
    if (mediaPlayer)
    {
        // Music
		Platform::String^ p_string = ref new Platform::String(wString);
		mediaPlayer->SetSource(p_string);
        m_sounds.push_back(new Win81Sound(soundId, -1, mediaPlayer));

		delete p_string;
    }
    else
    {
        // Sound
        for (int i = 0; i < m_iNumInstances; ++i)
        {
			Platform::String^ p_string = ref new Platform::String(wString);
            SoundFileReader sound(p_string);
            int soundIndex = WIN_8_1_AUDIO_ENGINE_HELPER->getSoundPlayerInstance()->AddSound(sound.GetSoundFormat(), sound.GetSoundData());
            m_sounds.push_back(new Win81Sound(soundId, soundIndex));

			delete p_string;
        }
    }
    
    delete wString;
    delete wavFileName;
}

Win81SoundWrapper::~Win81SoundWrapper()
{
    // Empty
}
