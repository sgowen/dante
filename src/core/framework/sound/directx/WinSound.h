//
//  WinSound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__WinSound__
#define __noctisgames__WinSound__

#include "ISound.h"

#include "Audio.h"

#include <memory>

class WinSound : public ISound
{
public:
    WinSound(int soundId, DirectX::SoundEffect& sound, float volume = 1.0f);
    
    virtual ~WinSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    std::unique_ptr<DirectX::SoundEffectInstance> m_soundEffectInstance;
    bool m_isLooping;
    bool m_isPaused;
};

#endif /* defined(__noctisgames__WinSound__) */
