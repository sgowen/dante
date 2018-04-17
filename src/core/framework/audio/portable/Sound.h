//
//  Sound.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Sound__
#define __noctisgames__Sound__

class Sound
{
public:
    Sound(int soundId);
    
    virtual ~Sound();
    
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
    int _soundId;
};

#endif /* defined(__noctisgames__Sound__) */
