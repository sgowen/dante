//
//  DirectXSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/sound/directx/DirectXSoundWrapper.h"

#include "framework/sound/directx/DirectXSound.h"

#include "framework/util/NGSTDUtil.h"

DirectXSoundWrapper::DirectXSoundWrapper(int soundId, const char *path, DirectX::AudioEngine* audioEngine, int numInstances) : SoundWrapper(soundId, numInstances)
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
    
    _sound = std::make_unique<DirectX::SoundEffect>(audioEngine, wString);
    
    delete wString;
    delete wavFileName;
    
    for (int i = 0; i < _numInstances; ++i)
    {
        _sounds.push_back(new DirectXSound(soundId, *_sound));
    }
}

DirectXSoundWrapper::~DirectXSoundWrapper()
{
	NGSTDUtil::cleanUpVectorOfPointers(_sounds);

    if (_sound)
    {
        _sound.reset();
    }
}
