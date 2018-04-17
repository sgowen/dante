//
//  NullSoundWrapper.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NullSoundWrapper__
#define __noctisgames__NullSoundWrapper__

#include <framework/audio/portable/SoundWrapper.h>

class NullSoundWrapper : public SoundWrapper
{
public:
    NullSoundWrapper(int soundId, const char *path, int numInstances = 1);
    
    virtual ~NullSoundWrapper();
};

#endif /* defined(__noctisgames__NullSoundWrapper__) */
