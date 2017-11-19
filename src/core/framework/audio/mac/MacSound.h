//
//  MacSound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MacSound__
#define __noctisgames__MacSound__

#include "framework/audio/portable/Sound.h"

class MacSound : public Sound
{
public:
    MacSound(int soundId, const char *path, float volume = 1.0f);
    
    virtual ~MacSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    int _objectALSoundIndex;
    float _volume;
    bool _isLooping;
    bool _isPaused;
    bool _isMusic;
};

#endif /* defined(__noctisgames__MacSound__) */
