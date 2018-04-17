//
//  NullSound.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NullSound__
#define __noctisgames__NullSound__

#include <framework/audio/portable/Sound.h>

class NullSound : public Sound
{
public:
    NullSound(int soundId, const char *path, float volume = 1.0f);
    
    virtual ~NullSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    float _volume;
    bool _isLooping;
    bool _isPaused;
    bool _isMusic;
};

#endif /* defined(__noctisgames__NullSound__) */
