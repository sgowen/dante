//
//  OpenSLESSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenSLESSoundWrapper__
#define __noctisgames__OpenSLESSoundWrapper__

#include "framework/audio/portable/SoundWrapper.h"

class OpenSLESManager;

class OpenSLESSoundWrapper : public SoundWrapper
{
public:
    OpenSLESSoundWrapper(OpenSLESManager* openSLESManager, int soundId, const char *path, int numInstances = 1);
    
    virtual ~OpenSLESSoundWrapper();
};

#endif /* defined(__noctisgames__OpenSLESSoundWrapper__) */
