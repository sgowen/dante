//
//  AndroidSound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidSound__
#define __noctisgames__AndroidSound__

#include "framework/audio/portable/Sound.h"

class SoundService;
class OpenSLESSound;

class AndroidSound : public Sound
{
public:
    AndroidSound(SoundService* soundService, int soundId, const char *path, float volume = 1.0f);
    
    virtual ~AndroidSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    SoundService* _soundService;
    OpenSLESSound* _sound;
    const char* _path;
    float _volume;
    bool _isLooping;
    bool _isPaused;
    bool _isMusic;
};

#endif /* defined(__noctisgames__AndroidSound__) */
