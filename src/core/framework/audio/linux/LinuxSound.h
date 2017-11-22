//
//  LinuxSound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LinuxSound__
#define __noctisgames__LinuxSound__

#include "framework/audio/portable/Sound.h"

#include <AL/al.h>

class LinuxSound : public Sound
{
public:
    LinuxSound(int soundId, const char *path, float volume = 1.0f);
    
    virtual ~LinuxSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    ALuint buf;
    ALuint src;
    float _volume;
    bool _isLooping;
    bool _isPaused;
    bool _isMusic;
};

#endif /* defined(__noctisgames__LinuxSound__) */
