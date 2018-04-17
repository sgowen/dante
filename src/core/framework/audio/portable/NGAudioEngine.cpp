//
//  NGAudioEngine.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/portable/NGAudioEngine.h>

#include <framework/audio/portable/SoundWrapper.h>
#include <framework/audio/portable/Sound.h>
#include <framework/audio/portable/AudioEngineHelper.h>

#include <framework/audio/portable/AudioEngineHelperFactory.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/file/portable/Assets.h>
#include <framework/audio/portable/SoundDesc.h>

#include <assert.h>

#define MAX_SOUNDS_TO_PLAY_PER_FRAME 3

NGAudioEngine* NGAudioEngine::s_instance = NULL;

void NGAudioEngine::create()
{
    assert(!s_instance);
    
    s_instance = new NGAudioEngine();
}

NGAudioEngine * NGAudioEngine::getInstance()
{
    return s_instance;
}

void NGAudioEngine::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void NGAudioEngine::update(int flags)
{
    _audioEngineHelper->update(flags);
}

void NGAudioEngine::render()
{
    if (_state == AudioEngineState_Resume)
    {
        _audioEngineHelper->resume();
    }
    
    {
        int len = static_cast<int>(_soundsToPlay.size());
        for (int i = 0; i < len; ++i)
        {
            Sound* sound = _soundsToPlay[i];
            float volume = _soundsVolumes[i];
            bool isLooping = _soundsLooping[i];
            
            sound->setVolume(volume);
            sound->play(isLooping);
        }
        _soundsToPlay.clear();
        _soundsVolumes.clear();
        _soundsLooping.clear();
    }
    
    {
        int len = static_cast<int>(_soundsToStop.size());
        for (int i = 0; i < len; ++i)
        {
            Sound* sound = _soundsToStop[i];
            sound->stop();
        }
        _soundsToStop.clear();
    }
    
    {
        int len = static_cast<int>(_soundsToPause.size());
        for (int i = 0; i < len; ++i)
        {
            Sound* sound = _soundsToPause[i];
            sound->pause();
        }
        _soundsToPause.clear();
    }
    
    {
        int len = static_cast<int>(_soundsToResume.size());
        for (int i = 0; i < len; ++i)
        {
            Sound* sound = _soundsToResume[i];
            sound->resume();
        }
        _soundsToResume.clear();
    }
    
    {
        int len = static_cast<int>(_musicStates.size());
        for (int i = 0; i < len; ++i)
        {
            int musicState = _musicStates[i];
            switch (musicState)
            {
                case MusicState_Stop:
                    _music->getSoundInstance()->stop();
                    break;
                case MusicState_Pause:
                    _music->getSoundInstance()->pause();
                    break;
                case MusicState_Resume:
                    if (_music->getSoundInstance()->isPaused())
                    {
                        _music->getSoundInstance()->resume();
                    }
                    break;
                case MusicState_SetVolume:
                    _music->getSoundInstance()->setVolume(_musicVolume);
                    break;
                case MusicState_Play:
                    _music->getSoundInstance()->play(_isMusicLooping);
                    _music->getSoundInstance()->setVolume(_musicVolume);
                    break;
                case MusicState_None:
                default:
                    break;
            }
        }
        _musicStates.clear();
    }
    
    if (_state == AudioEngineState_Pause)
    {
        _audioEngineHelper->pause();
    }
}

void NGAudioEngine::pause()
{
    _state = AudioEngineState_Pause;
    pauseMusic();
    pauseAllSounds();
}

void NGAudioEngine::resume()
{
    _state = AudioEngineState_Resume;
    resumeMusic();
    resumeAllSounds();
}

void NGAudioEngine::loadFromAssets()
{
    std::vector<SoundDesc*>& soundDescs = ASSETS->getSoundDescriptors();
    for (SoundDesc* sd : soundDescs)
    {
        int soundId = sd->_soundId;
        if (soundId == 1337)
        {
            loadMusic(sd->_soundFilePath.c_str());
        }
        else
        {
            loadSound(soundId, sd->_soundFilePath.c_str(), sd->_numInstances);
        }
    }
}

void NGAudioEngine::loadSound(int soundId, const char *path, int numInstances)
{
    SoundWrapper* sound = _audioEngineHelper->loadSound(soundId, path, numInstances);
    _sounds.insert(std::make_pair(soundId, sound));
}

void NGAudioEngine::playSound(int soundId, float inVolume, bool isLooping)
{
    if (_areSoundsDisabled
        || soundId <= 0
        || _soundsToPlay.size() >= MAX_SOUNDS_TO_PLAY_PER_FRAME)
    {
        return;
    }
    
    SoundWrapper* soundWrapper = findSound(soundId);
    Sound* sound = soundWrapper->getSoundInstance();
    float volume = clamp(inVolume, 0, 1);
    
    _soundsToPlay.push_back(sound);
    _soundsVolumes.push_back(volume);
    _soundsLooping.push_back(isLooping);
}

void NGAudioEngine::stopSound(int soundId)
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    SoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        Sound* sound = soundWrapper->getSoundInstance();
        if (sound->isPlaying())
        {
            _soundsToStop.push_back(sound);
            
            return;
        }
    }
}

void NGAudioEngine::pauseSound(int soundId)
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    SoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        Sound* sound = soundWrapper->getSoundInstance();
        if (sound->isPlaying())
        {
            _soundsToPause.push_back(sound);
            
            return;
        }
    }
}

void NGAudioEngine::resumeSound(int soundId)
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    SoundWrapper* soundWrapper = findSound(soundId);
    
    for (int j = 0; j < soundWrapper->getNumInstances(); ++j)
    {
        Sound* sound = soundWrapper->getSoundInstance();
        if (sound->isPaused())
        {
            _soundsToResume.push_back(sound);
            
            return;
        }
    }
}

void NGAudioEngine::stopAllSounds()
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    for (std::map<int, SoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<Sound *> sounds = (*i).second->getSounds();
        for (std::vector<Sound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            _soundsToStop.push_back((*i));
        }
    }
}

void NGAudioEngine::pauseAllSounds()
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    for (std::map<int, SoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<Sound *> sounds = (*i).second->getSounds();
        for (std::vector<Sound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            _soundsToPause.push_back((*i));
        }
    }
}

void NGAudioEngine::resumeAllSounds()
{
    if (_areSoundsDisabled)
    {
        return;
    }
    
    for (std::map<int, SoundWrapper*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
    {
        std::vector<Sound *> sounds = (*i).second->getSounds();
        for (std::vector<Sound *>::iterator i = sounds.begin(); i != sounds.end(); ++i)
        {
            _soundsToResume.push_back((*i));
        }
    }
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
        _musicVolume = clamp(inVolume, 0, 1);
        _isMusicLooping = isLooping;
        _musicStates.push_back(MusicState_Play);
    }
}

void NGAudioEngine::setMusicVolume(float inVolume)
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _musicVolume = clamp(inVolume, 0, 1);
        _musicStates.push_back(MusicState_SetVolume);
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
        _musicStates.push_back(MusicState_Stop);
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
        _musicStates.push_back(MusicState_Pause);
    }
}

void NGAudioEngine::resumeMusic()
{
    if (_isMusicDisabled)
    {
        return;
    }
    
    if (_music)
    {
        _musicStates.push_back(MusicState_Resume);
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

void NGAudioEngine::setMusicDisabled(bool inValue)
{
    if (!_isMusicDisabled && inValue)
    {
        stopMusic();
        _isMusicDisabled = true;
    }
    else if (_isMusicDisabled && !inValue)
    {
        _isMusicDisabled = false;
        playMusic();
    }
}

bool NGAudioEngine::areSoundsDisabled()
{
    return _areSoundsDisabled;
}

void NGAudioEngine::setSoundsDisabled(bool inValue)
{
    if (!_areSoundsDisabled && inValue)
    {
        stopAllSounds();
    }
    
    _areSoundsDisabled = inValue;
}

SoundWrapper* NGAudioEngine::findSound(int soundId)
{
    auto q = _sounds.find(soundId);
    
    assert(q != _sounds.end());
    
    SoundWrapper* sound = q->second;
    
    return sound;
}

NGAudioEngine::NGAudioEngine() :
_audioEngineHelper(NG_AUDIO_ENGINE_HELPER_FACTORY->createAudioEngineHelper()),
_music(NULL),
_state(AudioEngineState_None),
_musicVolume(0),
_isMusicLooping(false),
_isMusicDisabled(false),
_areSoundsDisabled(false)
{
    // Empty
}

NGAudioEngine::~NGAudioEngine()
{
    NGSTDUtil::cleanUpMapOfPointers(_sounds);
    
    if (_music)
    {
        delete _music;
        _music = NULL;
    }
    
    delete _audioEngineHelper;
}
