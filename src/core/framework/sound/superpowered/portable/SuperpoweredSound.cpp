//
//  SuperpoweredSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/20/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "SuperpoweredSound.h"

#include "SuperpoweredSoundManager.h"
#include "SuperpoweredAdvancedAudioPlayer.h"

#include <SuperpoweredSimple.h>

#include <math.h>

#define HEADROOM_DECIBEL 3.0f

static const float headroom = powf(10.0f, -HEADROOM_DECIBEL * 0.025f);

static void playerEventCallback(void *clientData, SuperpoweredAdvancedAudioPlayerEvent event, void * __unused value)
{
    SuperpoweredSound *sps = (SuperpoweredSound *)clientData;

    if (event == SuperpoweredAdvancedAudioPlayerEvent_LoadSuccess)
    {
    	sps->getPlayer()->setFirstBeatMs(0);
        sps->getPlayer()->setPosition(sps->getPlayer()->firstBeatMs, false, false);
    }
    else if (event == SuperpoweredAdvancedAudioPlayerEvent_EOF)
    {
        bool *pBoolValue = (bool *)value;
        *pBoolValue = !sps->isLooping();
        
        if (!sps->isLooping())
        {
            sps->getManager()->onSoundStopped(sps);
        }
    }
}

SuperpoweredSound::SuperpoweredSound(SuperpoweredSoundManager* manager, int soundId, const char *path, unsigned int sampleRate, int fileOffset, int fileLength, float volume) : ISound(soundId),
m_manager(manager),
m_fVolume(volume * headroom),
m_iLastSamplerate(sampleRate),
m_isLooping(false),
m_isPaused(false)
{
    m_player = new SuperpoweredAdvancedAudioPlayer(this, playerEventCallback, sampleRate, 0);
    
    if (fileOffset > -1 && fileLength > -1)
    {
        m_player->open(path, fileOffset, fileLength);
    }
    else
    {
        m_player->open(path);
    }
}

SuperpoweredSound::~SuperpoweredSound()
{
    m_player->pause();
    
    m_manager->onSoundStopped(this);
    
    delete m_player;
}

void SuperpoweredSound::play(bool isLooping)
{
    m_isLooping = isLooping;
    m_isPaused = false;
    
    m_player->seek(0);
    
    m_player->play(false);
    
    m_manager->onSoundPlayed(this);
}

void SuperpoweredSound::resume()
{
    if (m_isPaused)
    {
        m_player->play(false);
        
        m_isPaused = false;
    }
}

void SuperpoweredSound::pause()
{
    if (isPlaying())
    {
        m_player->pause();
        
        m_isPaused = true;
    }
}

void SuperpoweredSound::stop()
{
    m_isLooping = false;
    m_isPaused = false;
    
    m_player->pause();
    m_player->setFirstBeatMs(0);
    m_player->setPosition(m_player->firstBeatMs, false, false);
    
    m_manager->onSoundStopped(this);
}

void SuperpoweredSound::setVolume(float volume)
{
    m_fVolume = volume * headroom;
}

bool SuperpoweredSound::isLooping()
{
    return m_isLooping;
}

bool SuperpoweredSound::isPlaying()
{
    return m_player->playing;
}

bool SuperpoweredSound::isPaused()
{
    return m_isPaused;
}

bool SuperpoweredSound::process(float *stereoBuffer, void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    if (sampleRate != -1
        && sampleRate != m_iLastSamplerate)
    {
        // Has samplerate changed?
        m_iLastSamplerate = sampleRate;
        m_player->setSamplerate(sampleRate);
    }
    
    bool ret = m_player->process(stereoBuffer, false, numberOfSamples, m_fVolume);
    
    // The stereoBuffer is ready now, let's put the finished audio into the requested buffers.
    if (ret)
    {
#if defined __APPLE__
        float **buffers = (float **)output;
        SuperpoweredDeInterleave(stereoBuffer, buffers[0], buffers[1], numberOfSamples); // The stereoBuffer is ready now, let's put the finished audio into the requested buffers.
#elif defined __ANDROID__
        short int *realOutput = (short int *)output;
        SuperpoweredFloatToShortInt(stereoBuffer, realOutput, numberOfSamples);
#endif
    }
    
    return ret;
}

SuperpoweredAdvancedAudioPlayer* SuperpoweredSound::getPlayer()
{
    return m_player;
}

SuperpoweredSoundManager* SuperpoweredSound::getManager()
{
    return m_manager;
}
