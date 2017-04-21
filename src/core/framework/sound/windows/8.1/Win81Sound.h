//
//  Win81Sound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Win81Sound__
#define __noctisgames__Win81Sound__

#include "ISound.h"

class MediaEnginePlayer;

class Win81Sound : public ISound
{
public:
    Win81Sound(int soundId, MediaEnginePlayer* mediaPlayer, float volume = 1.0f);
    
    virtual ~Win81Sound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();

private:
    MediaEnginePlayer* m_mediaPlayer;
    bool m_isLooping;
    bool m_isPaused;
};

#endif /* defined(__noctisgames__Win81Sound__) */
