//
//  NullAudioEngineHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NullAudioEngineHelper__
#define __noctisgames__NullAudioEngineHelper__

#include <framework/audio/portable/AudioEngineHelper.h>

class NullAudioEngineHelper : public AudioEngineHelper
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
    NullAudioEngineHelper();
    virtual ~NullAudioEngineHelper();
    NullAudioEngineHelper(const NullAudioEngineHelper&);
    NullAudioEngineHelper& operator=(const NullAudioEngineHelper&);
};

#endif /* defined(__noctisgames__NullAudioEngineHelper__) */
