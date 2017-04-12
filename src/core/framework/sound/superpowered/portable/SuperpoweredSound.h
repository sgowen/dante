//
//  SuperpoweredSound.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SuperpoweredSound__
#define __noctisgames__SuperpoweredSound__

#include "ISound.h"

class SuperpoweredSoundManager;
class SuperpoweredAdvancedAudioPlayer;

class SuperpoweredSound : public ISound
{
public:
    SuperpoweredSound(SuperpoweredSoundManager* manager, int soundId, const char *path, unsigned int sampleRate, int fileOffset = -1, int fileLength = -1, float volume = 1.0f);
    
    virtual ~SuperpoweredSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();
    
    // output is float **buffers for iOS and Mac OS X and short int * for Android
    bool process(float *stereoBuffer, void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    
    SuperpoweredAdvancedAudioPlayer* getPlayer();
    
    SuperpoweredSoundManager* getManager();

private:
    SuperpoweredSoundManager* m_manager;
    SuperpoweredAdvancedAudioPlayer* m_player;
    float m_fVolume;
    unsigned int m_iLastSamplerate;
    bool m_isLooping;
    bool m_isPaused;
};

#endif /* defined(__noctisgames__SuperpoweredSound__) */
