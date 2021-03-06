//
//  AndroidAudioEngineHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAudioEngineHelper__
#define __noctisgames__AndroidAudioEngineHelper__

#include <framework/audio/portable/AudioEngineHelper.h>

#include <vector>

class AndroidAudioEngineHelper : public AudioEngineHelper
{
    friend class AudioEngineHelperFactory;
    
public:
    virtual void update(int flags = 0);
    virtual void pause();
    virtual void resume();
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    virtual SoundWrapper* loadMusic(const char* path);
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAudioEngineHelper();
    ~AndroidAudioEngineHelper();
    AndroidAudioEngineHelper(const AndroidAudioEngineHelper&);
    AndroidAudioEngineHelper& operator=(const AndroidAudioEngineHelper&);
};

#endif /* defined(__noctisgames__AndroidAudioEngineHelper__) */
