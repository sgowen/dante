//
//  AndroidSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidSoundWrapper__
#define __noctisgames__AndroidSoundWrapper__

#include "framework/audio/portable/SoundWrapper.h"

class SoundService;

class AndroidSoundWrapper : public SoundWrapper
{
public:
    AndroidSoundWrapper(SoundService* soundService, int soundId, const char *path, int numInstances = 1);
    
    virtual ~AndroidSoundWrapper();
};

#endif /* defined(__noctisgames__AndroidSoundWrapper__) */
