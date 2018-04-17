//
//  AudioEngineHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AudioEngineHelper__
#define __noctisgames__AudioEngineHelper__

class SoundWrapper;

class AudioEngineHelper
{
public:
    AudioEngineHelper();
    virtual ~AudioEngineHelper();
    
    virtual void update(int flags = 0) = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1) = 0;
    virtual SoundWrapper* loadMusic(const char* path) = 0;
};

#endif /* defined(__noctisgames__AudioEngineHelper__) */
