//
//  SuperpoweredSoundManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SuperpoweredSoundManager__
#define __noctisgames__SuperpoweredSoundManager__

#include <vector>

#define MAX_NUM_SOUND_PLAYERS 12

class SuperpoweredSound;
class SuperpoweredSoundWrapper;

class SuperpoweredSoundManager
{
public:
    SuperpoweredSoundManager(unsigned int sampleRate = 44100, unsigned int bufferSize = 0);
    
	~SuperpoweredSoundManager();
    
    void onSoundPlayed(SuperpoweredSound* sound);
    
    void onSoundStopped(SuperpoweredSound* sound);
    
    bool processMusic(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound1(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound2(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound3(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound4(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound5(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound6(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound7(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound8(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound9(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound10(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);
    bool processSound11(void *output, unsigned int numberOfSamples, unsigned int sampleRate = -1);

private:
    std::vector<float*> m_stereoBuffers;
    SuperpoweredSound *m_activeSounds[MAX_NUM_SOUND_PLAYERS];
    int m_iSampleRate;
    int m_iSoundIndex;
    
    bool processSound(void *output, unsigned int numberOfSamples, SuperpoweredSound *sound, float *stereoBuffer, unsigned int sampleRate = -1);
};

#endif /* defined(__noctisgames__SuperpoweredSound__) */
