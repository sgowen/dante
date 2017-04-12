//
//  NGAudioEngine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "NGAudioEngine.h"

#include "ISoundWrapper.h"
#include "ISound.h"
#include "IAudioEngineHelper.h"

#include "AudioEngineHelperFactory.h"
#include "NGSTDUtil.h"

#include <assert.h>

#define MAX_SOUNDS_TO_PLAY_PER_FRAME 3

NGAudioEngine* NGAudioEngine::getInstance()
{
    static NGAudioEngine instance = NGAudioEngine();
    return &instance;
}

void NGAudioEngine::update(int flags)
{
    m_iNumSoundsPlayedThisFrame = 0;
    
    m_audioEngineHelper->update(flags);
}

void NGAudioEngine::pause()
{
    pauseMusic();
    pauseAllSounds();
    
    m_audioEngineHelper->pause();
}

void NGAudioEngine::resume()
{
    m_audioEngineHelper->resume();
    
    resumeMusic();
    resumeAllSounds();
}

void NGAudioEngine::loadSound(int soundId, const char *path, int numInstances)
{
    ISoundWrapper* sound = m_audioEngineHelper->loadSound(soundId, path, numInstances);
    
    m_sounds.insert(std::make_pair(soundId, sound));
}

void NGAudioEngine::playSound(int soundId, bool isLooping, float volume)
{
    if (m_isSoundDisabled
        || soundId <= 0
        || m_iNumSoundsPlayedThisFrame >= MAX_SOUNDS_TO_PLAY_PER_FRAME)
    {
        return;
    }
    
    ISoundWrapper* soundWrapper = findSound(soundId);
    
    ISound* sound = soundWrapper->getSoundInstance();
    
    sound->play(isLooping);
}

void NGAudioEngine::stopSound(int soundId)
{
    if (m_isSoundDisabled)
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
    if (m_isSoundDisabled)
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
    if (m_isSoundDisabled)
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
    if (m_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = m_sounds.begin(); i != m_sounds.end(); ++i)
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
    if (m_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = m_sounds.begin(); i != m_sounds.end(); ++i)
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
    if (m_isSoundDisabled)
    {
        return;
    }
    
    for (std::map<int, ISoundWrapper*>::iterator i = m_sounds.begin(); i != m_sounds.end(); ++i)
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
    NGSTDUtil::cleanUpMapOfPointerValues(m_sounds);
}

void NGAudioEngine::loadMusic(const char *path)
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music)
    {
        m_music->getSoundInstance()->stop();
        
        delete m_music;
    }
    
    m_music = m_audioEngineHelper->loadMusic(path);
}

void NGAudioEngine::playMusic(bool isLooping, float volume)
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music)
    {
        m_music->getSoundInstance()->play(isLooping);
        m_music->getSoundInstance()->setVolume(volume);
    }
}

void NGAudioEngine::setMusicVolume(float volume)
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music)
    {
        m_music->getSoundInstance()->setVolume(volume);
    }
}

void NGAudioEngine::stopMusic()
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music)
    {
        m_music->getSoundInstance()->stop();
    }
}

void NGAudioEngine::pauseMusic()
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music)
    {
        m_music->getSoundInstance()->pause();
    }
}

void NGAudioEngine::resumeMusic()
{
    if (m_isMusicDisabled)
    {
        return;
    }
    
    if (m_music && m_music->getSoundInstance()->isPaused())
    {
        m_music->getSoundInstance()->resume();
    }
}

bool NGAudioEngine::isMusicLoaded()
{
    return m_music ? true : false;
}

bool NGAudioEngine::isMusicDisabled()
{
    return m_isMusicDisabled;
}

void NGAudioEngine::setMusicDisabled(bool isMusicDisabled)
{
    if (m_isMusicDisabled && !isMusicDisabled)
    {
        m_isMusicDisabled = false;
        playMusic();
    }
    else if (!m_isMusicDisabled && isMusicDisabled)
    {
        stopMusic();
        m_isMusicDisabled = true;
    }
}

bool NGAudioEngine::isSoundDisabled()
{
    return m_isSoundDisabled;
}

void NGAudioEngine::setSoundDisabled(bool isSoundDisabled)
{
    m_isSoundDisabled = isSoundDisabled;
}

ISoundWrapper* NGAudioEngine::findSound(int soundId)
{
    auto q = m_sounds.find(soundId);
    
    assert(q != m_sounds.end());
    
    ISoundWrapper* sound = q->second;
    
    return sound;
}

NGAudioEngine::NGAudioEngine() :
m_music(nullptr),
m_audioEngineHelper(NG_AUDIO_ENGINE_HELPER_FACTORY->createAudioEngineHelper()),
m_iNumSoundsPlayedThisFrame(0),
m_isMusicDisabled(false),
m_isSoundDisabled(false)
{
    // Empty
}

NGAudioEngine::~NGAudioEngine()
{
    resetSounds();
    
    delete m_music;
}
