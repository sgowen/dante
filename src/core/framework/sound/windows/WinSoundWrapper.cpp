//
//  WinSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "WinSoundWrapper.h"

#include "WinSound.h"

#include "NGSTDUtil.h"

WinSoundWrapper::WinSoundWrapper(int soundId, const char *path, DirectX::AudioEngine* audioEngine, int numInstances) : ISoundWrapper(soundId, numInstances)
{
    size_t len = strlen(path);
    
    char* wavFileName = new char[len + 5];
    
	strcpy_s(wavFileName, len + 6, path);
    wavFileName[len] = '.';
    wavFileName[len + 1] = 'w';
    wavFileName[len + 2] = 'a';
    wavFileName[len + 3] = 'v';
    wavFileName[len + 4] = '\0';
    
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, wavFileName, -1, wString, 4096);
    
    m_sound = std::make_unique<DirectX::SoundEffect>(audioEngine, wString);
    
    delete wString;
    delete wavFileName;
    
    for (int i = 0; i < m_iNumInstances; ++i)
    {
        m_sounds.push_back(new WinSound(soundId, *m_sound));
    }
}

WinSoundWrapper::~WinSoundWrapper()
{
    if (m_sound)
    {
        m_sound.reset();
    }
}
