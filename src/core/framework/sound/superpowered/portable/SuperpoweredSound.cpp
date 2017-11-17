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
_manager(manager),
_volume(volume * headroom),
_lastSamplerate(sampleRate),
_isLooping(false),
_isPaused(false)
{
    _player = new SuperpoweredAdvancedAudioPlayer(this, playerEventCallback, sampleRate, 0);
    
    if (fileOffset > -1 && fileLength > -1)
    {
        _player->open(path, fileOffset, fileLength);
    }
    else
    {
        _player->open(path);
    }
}

SuperpoweredSound::~SuperpoweredSound()
{
    _player->pause();
    
    _manager->onSoundStopped(this);
    
    delete _player;
}

void SuperpoweredSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
    
    _player->seek(0);
    
    _player->play(false);
    
    _manager->onSoundPlayed(this);
}

void SuperpoweredSound::resume()
{
    if (_isPaused)
    {
        _player->play(false);
        
        _isPaused = false;
    }
}

void SuperpoweredSound::pause()
{
    if (isPlaying())
    {
        _player->pause();
        
        _isPaused = true;
    }
}

void SuperpoweredSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    _player->pause();
    _player->setFirstBeatMs(0);
    _player->setPosition(_player->firstBeatMs, false, false);
    
    _manager->onSoundStopped(this);
}

void SuperpoweredSound::setVolume(float volume)
{
    _volume = volume * headroom;
}

bool SuperpoweredSound::isLooping()
{
    return _isLooping;
}

bool SuperpoweredSound::isPlaying()
{
    return _player->playing;
}

bool SuperpoweredSound::isPaused()
{
    return _isPaused;
}

bool SuperpoweredSound::process(float *stereoBuffer, void *output, unsigned int numberOfSamples, unsigned int sampleRate)
{
    if (sampleRate != -1
        && sampleRate != _lastSamplerate)
    {
        // Has samplerate changed?
        _lastSamplerate = sampleRate;
        _player->setSamplerate(sampleRate);
    }
    
    bool ret = _player->process(stereoBuffer, false, numberOfSamples, _volume);
    
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
    return _player;
}

SuperpoweredSoundManager* SuperpoweredSound::getManager()
{
    return _manager;
}
