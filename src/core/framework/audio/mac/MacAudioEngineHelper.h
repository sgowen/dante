//
//  MacAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MacAudioEngineHelper__
#define __noctisgames__MacAudioEngineHelper__

#define MAC_AUDIO_ENGINE_HELPER (MacAudioEngineHelper::getInstance())

#include "framework/audio/portable/AudioEngineHelper.h"

class MacAudioEngineHelper : public AudioEngineHelper
{
public:
	static MacAudioEngineHelper* getInstance();
    
    virtual void update(int flags = 0);
    
    virtual void pause();
    
    virtual void resume();
    
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    
    virtual SoundWrapper* loadMusic(const char* path);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    MacAudioEngineHelper();
    virtual ~MacAudioEngineHelper();
    MacAudioEngineHelper(const MacAudioEngineHelper&);
    MacAudioEngineHelper& operator=(const MacAudioEngineHelper&);
};

#endif /* defined(__noctisgames__MacAudioEngineHelper__) */
