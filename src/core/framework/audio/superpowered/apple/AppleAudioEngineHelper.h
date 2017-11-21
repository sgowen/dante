//
//  AppleAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AppleAudioEngineHelper__
#define __noctisgames__AppleAudioEngineHelper__

#include "framework/audio/portable/AudioEngineHelper.h"

class SuperpoweredSoundManager;

class AppleAudioEngineHelper : public AudioEngineHelper
{
public:
	static AppleAudioEngineHelper* getInstance();
    
    virtual void update(int flags = 0);
    
    virtual void pause();
    
    virtual void resume();
    
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    
    virtual SoundWrapper* loadMusic(const char* path);

private:
    SuperpoweredSoundManager* _superpoweredSoundManager;
    int _sampleRate;

    // ctor, copy ctor, and assignment should be private in a Singleton
    AppleAudioEngineHelper();
    virtual ~AppleAudioEngineHelper();
    AppleAudioEngineHelper(const AppleAudioEngineHelper&);
    AppleAudioEngineHelper& operator=(const AppleAudioEngineHelper&);
};

#endif /* defined(__noctisgames__AppleAudioEngineHelper__) */
