//
//  SoundWrapper.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SoundWrapper__
#define __noctisgames__SoundWrapper__

#include <vector>

class Sound;

class SoundWrapper
{
public:
    SoundWrapper(int soundId, int numInstances = 1);
    
    virtual ~SoundWrapper();
    
    Sound* getSoundInstance();
    
    std::vector<Sound *> getSounds();
    
    int getSoundIndex();
    
    int getSoundId();
    
    int getNumInstances();
    
protected:
    std::vector<Sound *> _sounds;
    int _soundIndex;
    int _soundId;
    int _numInstances;
};

#endif /* defined(__noctisgames__SoundWrapper__) */
