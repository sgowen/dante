//
//  DirectXAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXAudioEngineHelper__
#define __noctisgames__DirectXAudioEngineHelper__

#include "framework/audio/portable/AudioEngineHelper.h"

#include "Audio.h"

#include <memory>

class DirectXAudioEngineHelper : public AudioEngineHelper
{
    friend class AudioEngineHelperFactory;
    
public:
	virtual void update(int flags = 0);
    virtual void pause();
    virtual void resume();
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    virtual SoundWrapper* loadMusic(const char* path);

private:
    std::unique_ptr<DirectX::AudioEngine> _audEngine;
    bool _retryAudio;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    DirectXAudioEngineHelper();
    virtual ~DirectXAudioEngineHelper();
    DirectXAudioEngineHelper(const DirectXAudioEngineHelper&);
    DirectXAudioEngineHelper& operator=(const DirectXAudioEngineHelper&);
};

#endif /* defined(__noctisgames__DirectXAudioEngineHelper__) */
