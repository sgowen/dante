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

Win81SoundWrapper::Win81SoundWrapper(int soundId, const char *path, MediaEnginePlayer* mediaPlayer, int numInstances) : ISoundWrapper(soundId, numInstances)
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
    
    // TODO
    
    delete wString;
    delete wavFileName;
    
    for (int i = 0; i < m_iNumInstances; ++i)
    {
        m_sounds.push_back(new Win81Sound(soundId, mediaPlayer, *m_sound));
    }
}

Win81SoundWrapper::~Win81SoundWrapper()
{
    // Empty
}
