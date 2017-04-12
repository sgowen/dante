//
//  ISoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ISoundWrapper__
#define __noctisgames__ISoundWrapper__

#include <vector>

class ISound;

class ISoundWrapper
{
public:
    ISoundWrapper(int soundId, int numInstances = 1);
    
    virtual ~ISoundWrapper();
    
    ISound* getSoundInstance();
    
    std::vector<ISound *> getSounds();
    
    int getSoundIndex();
    
    int getSoundId();
    
    int getNumInstances();
    
protected:
    std::vector<ISound *> m_sounds;
    int m_iSoundIndex;
    int m_iSoundId;
    int m_iNumInstances;
};

#endif /* defined(__noctisgames__ISoundWrapper__) */
