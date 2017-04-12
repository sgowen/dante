//
//  SuperpoweredSoundManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SuperpoweredSoundWrapper__
#define __noctisgames__SuperpoweredSoundWrapper__

#include "ISoundWrapper.h"

#include <vector>

class SuperpoweredSoundManager;
class SuperpoweredSound;

class SuperpoweredSoundWrapper : public ISoundWrapper
{
public:
    SuperpoweredSoundWrapper(SuperpoweredSoundManager* manager, int soundId, const char *path, unsigned int sampleRate, int numInstances = 1, int fileOffset = -1, int fileLength = -1);
};

#endif /* defined(__noctisgames__SuperpoweredSoundWrapper__) */
