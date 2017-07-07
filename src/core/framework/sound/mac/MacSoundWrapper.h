//
//  MacSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MacSoundWrapper__
#define __noctisgames__MacSoundWrapper__

#include "ISoundWrapper.h"

class MacSoundWrapper : public ISoundWrapper
{
public:
    MacSoundWrapper(int soundId, const char *path, int numInstances = 1);
    
    virtual ~MacSoundWrapper();
};

#endif /* defined(__noctisgames__MacSoundWrapper__) */
