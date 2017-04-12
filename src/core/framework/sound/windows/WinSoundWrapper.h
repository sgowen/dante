//
//  WinSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__WinSoundWrapper__
#define __noctisgames__WinSoundWrapper__

#include "ISoundWrapper.h"

#include "Audio.h"

#include <memory>

class WinSoundWrapper : public ISoundWrapper
{
public:
    WinSoundWrapper(int soundId, const char *path, DirectX::AudioEngine* audioEngine, int numInstances = 1);
    
    virtual ~WinSoundWrapper();
    
private:
    std::unique_ptr<DirectX::SoundEffect> m_sound;
};

#endif /* defined(__noctisgames__WinSoundWrapper__) */
