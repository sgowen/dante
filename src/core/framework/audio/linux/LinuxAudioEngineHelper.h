//
//  LinuxAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LinuxAudioEngineHelper__
#define __noctisgames__LinuxAudioEngineHelper__

#include "framework/audio/portable/AudioEngineHelper.h"

class LinuxAudioEngineHelper : public AudioEngineHelper
{
public:
	static LinuxAudioEngineHelper* getInstance();

    virtual void update(int flags = 0);

    virtual void pause();

    virtual void resume();

    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);

    virtual SoundWrapper* loadMusic(const char* path);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    LinuxAudioEngineHelper();
    ~LinuxAudioEngineHelper();
    LinuxAudioEngineHelper(const LinuxAudioEngineHelper&);
    LinuxAudioEngineHelper& operator=(const LinuxAudioEngineHelper&);
};

#endif /* defined(__noctisgames__LinuxAudioEngineHelper__) */
