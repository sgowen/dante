//
//  DirectXSoundWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/directx/DirectXSoundWrapper.h"

#include "framework/audio/directx/DirectXSound.h"

#include "framework/util/NGSTDUtil.h"

DirectXSoundWrapper::DirectXSoundWrapper(int soundId, const char *path, DirectX::AudioEngine* audioEngine, int numInstances) : SoundWrapper(soundId, numInstances)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, path, -1, wString, 4096);
    
    _sound = std::make_unique<DirectX::SoundEffect>(audioEngine, wString);
    
    delete wString;
    
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
