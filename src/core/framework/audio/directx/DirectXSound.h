//
//  DirectXSound.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXSound__
#define __noctisgames__DirectXSound__

#include <framework/audio/portable/Sound.h>

#include "Audio.h"

#include <memory>

class DirectXSound : public Sound
{
public:
    DirectXSound(int soundId, DirectX::SoundEffect& sound, float volume = 1.0f);
    
    virtual ~DirectXSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    std::unique_ptr<DirectX::SoundEffectInstance> _soundEffectInstance;
    bool _isLooping;
    bool _isPaused;
};

#endif /* defined(__noctisgames__DirectXSound__) */
