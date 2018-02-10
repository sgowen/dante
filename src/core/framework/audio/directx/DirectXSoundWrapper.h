//
//  DirectXSoundWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXSoundWrapper__
#define __noctisgames__DirectXSoundWrapper__

#include <framework/audio/portable/SoundWrapper.h>

#include "Audio.h"

#include <memory>

class DirectXSoundWrapper : public SoundWrapper
{
public:
    DirectXSoundWrapper(int soundId, const char *path, DirectX::AudioEngine* audioEngine, int numInstances = 1);
    
    virtual ~DirectXSoundWrapper();
    
private:
    std::unique_ptr<DirectX::SoundEffect> _sound;
};

#endif /* defined(__noctisgames__DirectXSoundWrapper__) */
