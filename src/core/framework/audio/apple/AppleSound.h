//
//  AppleSound.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AppleSound__
#define __noctisgames__AppleSound__

#include <framework/audio/portable/Sound.h>

class AppleSound : public Sound
{
public:
    AppleSound(int soundId, const char *path, float volume = 1.0f);
    
    virtual ~AppleSound();

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

#endif /* defined(__noctisgames__AppleSound__) */
