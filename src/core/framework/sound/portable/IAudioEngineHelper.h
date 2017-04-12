//
//  IAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IAudioEngineHelper__
#define __noctisgames__IAudioEngineHelper__

class ISoundWrapper;

class IAudioEngineHelper
{
public:
    IAudioEngineHelper();
    
    virtual ~IAudioEngineHelper();
    
    virtual void update(int flags = 0) = 0;
    
    virtual void pause() = 0;
    
    virtual void resume() = 0;
    
    virtual ISoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1) = 0;
    
    virtual ISoundWrapper* loadMusic(const char* path) = 0;
};

#endif /* defined(__noctisgames__IAudioEngineHelper__) */
