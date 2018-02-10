//
//  AppleSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AppleSoundWrapper__
#define __noctisgames__AppleSoundWrapper__

#include <framework/audio/portable/SoundWrapper.h>

class AppleSoundWrapper : public SoundWrapper
{
public:
    AppleSoundWrapper(int soundId, const char *path, int numInstances = 1);
    
    virtual ~AppleSoundWrapper();
};

#endif /* defined(__noctisgames__AppleSoundWrapper__) */
