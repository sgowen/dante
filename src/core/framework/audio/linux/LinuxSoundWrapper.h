//
//  LinuxSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LinuxSoundWrapper__
#define __noctisgames__LinuxSoundWrapper__

#include "framework/audio/portable/SoundWrapper.h"

class LinuxSoundWrapper : public SoundWrapper
{
public:
    LinuxSoundWrapper(int soundId, const char *path, int numInstances = 1);
    
    virtual ~LinuxSoundWrapper();
};

#endif /* defined(__noctisgames__LinuxSoundWrapper__) */
