//
//  NGAudioEngine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGAudioEngine.h"

#include "ISoundWrapper.h"
#include "ISound.h"
#include "IAudioEngineHelper.h"

#include "AudioEngineHelperFactory.h"
#include "NGSTDUtil.h"
#include "MathUtil.h"

#include <assert.h>

#define MAX_SOUNDS_TO_PLAY_PER_FRAME 3

NGAudioEngine* NGAudioEngine::getInstance()
{
    static NGAudioEngine instance = NGAudioEngine();
    return &instance;
}

void NGAudioEngine::update(int flags)
{
    _numSoundsPlayedThisFrame = 0;
    
    _audioEngineHelper->update(flags);
}

void NGAudioEngine::pause()
{
    pauseMusic();
    pauseAllSounds();
    
    _audioEngineHelper->pause();
}

void NGAudioEngine::resume()
{
    _audioEngineHelper->resume();
    
    resumeMusic();
    resumeAllSounds();
}

void NGAudioEngine::loadSound(int soundId, const char *path, int numInstances)
{
    ISoundWrapper* sound = _audioEngineHelper->loadSound(soundId, path, numInstances);
    
    _sounds.insert(std::make_pair(soundId, sound));
}

void NGAudioEngine::playSound(int soundId, float inVolume, bool isLooping)
{
    if (_isSoundDisabled
        || soundId <= 0
        || _numSoundsPlayedThisFrame >= MAX_SOUNDS_TO_PLAY_PER_FRAME)
    {
        return;
    }
    
    ISoundWrapper* soundWrapper = findSound(soundId);
    
    ISound* sound = soundWrapper->getSoundInstance();
    
    float volume = clamp(inVolume, 1, 0);
    
    sound->setVolume(volume);
    sound->play(isLooping);
}

void NGAudioEngine::stopSound(int soundId)
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    ISoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        ISound* sound = soundWrapper->getSoundInstance();
        if (sound->isPlaying())
        {
            sound->stop();
            
            return;
        }
    }
}

void NGAudioEngine::pauseSound(int soundId)
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    ISoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        ISound* sound = soundWrapper->getSoundInstance();
        if (sound->isPlaying())
        {
            sound->pause();
            
            return;
        }
    }
}

void NGAudioEngine::resumeSound(int soundId)
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    ISoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        ISound* sound = soundWrapper->getSoundInstance();
        if (sound->isPaused())
        {
            sound->resume();
            
            return;
        }
    }
}

void NGAudioEngine::stopAllSounds()
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<ISound *> sounds = (*i).second->getSounds();
        for (std::vector<ISound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            (*i)->stop();
        }
    }
}

void NGAudioEngine::pauseAllSounds()
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<ISound *> sounds = (*i).second->getSounds();
        for (std::vector<ISound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            (*i)->pause();
        }
    }
}

void NGAudioEngine::resumeAllSounds()
{
    if (_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<ISound *> sounds = (*i).second->getSounds();
        for (std::vector<ISound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            (*i)->resume();
        }
    }
}

void NGAudioEngine::resetSounds()
{
    NGSTDUtil::cleanUpMapOfPointers(_sounds);
}

void NGAudioEngine::loadMusic(const char *path)
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _music->getSoundInstance()->stop();
        
        delete _music;
    }
    
    _music = _audioEngineHelper->loadMusic(path);
}

void NGAudioEngine::playMusic(bool isLooping, float inVolume)
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        float volume = clamp(inVolume, 1, 0);
        
        _music->getSoundInstance()->play(isLooping);
        _music->getSoundInstance()->setVolume(volume);
    }
}

void NGAudioEngine::setMusicVolume(float volume)
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _music->getSoundInstance()->setVolume(volume);
    }
}

void NGAudioEngine::stopMusic()
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _music->getSoundInstance()->stop();
    }
}

void NGAudioEngine::pauseMusic()
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _music->getSoundInstance()->pause();
    }
}

void NGAudioEngine::resumeMusic()
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music && _music->getSoundInstance()->isPaused())
    {
        _music->getSoundInstance()->resume();
    }
}

bool NGAudioEngine::isMusicPlaying()
{
    if (_music)
    {
        return _music->getSoundInstance()->isPlaying();
    }
    
    return false;
}

bool NGAudioEngine::isMusicLoaded()
{
    return _music ? true : false;
}

bool NGAudioEngine::isMusicDisabled()
{
    return _isMusicDisabled;
}

void NGAudioEngine::setMusicDisabled(bool isMusicDisabled)
{
    if (_isMusicDisabled && !isMusicDisabled)
    {
        _isMusicDisabled = false;
        playMusic();
    }
    else if (!_isMusicDisabled && isMusicDisabled)
    {
        stopMusic();
        _isMusicDisabled = true;
    }
}

bool NGAudioEngine::isSoundDisabled()
{
    return _isSoundDisabled;
}

void NGAudioEngine::setSoundDisabled(bool isSoundDisabled)
{
    _isSoundDisabled = isSoundDisabled;
}

ISoundWrapper* NGAudioEngine::findSound(int soundId)
{
    auto q = _sounds.find(soundId);
    
    assert(q != _sounds.end());
    
    ISoundWrapper* sound = q->second;
    
    return sound;
}

NGAudioEngine::NGAudioEngine() :
_music(nullptr),
_audioEngineHelper(NG_AUDIO_ENGINE_HELPER_FACTORY->createAudioEngineHelper()),
_numSoundsPlayedThisFrame(0),
_isMusicDisabled(false),
_isSoundDisabled(false)
{
    // Empty
}

NGAudioEngine::~NGAudioEngine()
{
    resetSounds();
    
    delete _music;
}
