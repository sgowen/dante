//
//  ISound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ISound__
#define __noctisgames__ISound__

class ISound
{
public:
    ISound(int soundId);
    
    virtual ~ISound();
    
    virtual void play(bool isLooping = false) = 0;
    
    virtual void resume() = 0;
    
    virtual void pause() = 0;
    
    virtual void stop() = 0;
    
    virtual void setVolume(float volume) = 0;
    
    virtual bool isLooping() = 0;
    
    virtual bool isPlaying() = 0;
    
    virtual bool isPaused() = 0;
    
    int getSoundId();
    
protected:
    int m_iSoundId;
};

#endif /* defined(__noctisgames__ISound__) */
