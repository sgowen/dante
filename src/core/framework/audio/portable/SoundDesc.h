//
//  SoundDesc.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/29/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SoundDesc__
#define __noctisgames__SoundDesc__

#include <string>

struct SoundDesc
{
    int _soundId;
    std::string _soundName;
    int _numInstances;
    
    SoundDesc(int inSoundId, std::string inSoundName, int inNumInstances) :
    _soundId(inSoundId),
    _soundName(inSoundName),
    _numInstances(inNumInstances)
    {
        // I normally don't allow definitions in headers, but sometimes I'm cool with it
    }
};

#endif /* defined(__noctisgames__SoundDesc__) */
