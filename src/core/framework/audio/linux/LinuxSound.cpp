//
//  LinuxSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/audio/linux/LinuxSound.h"

#include <assert.h>

#include <AL/alut.h>

LinuxSound::LinuxSound(int soundId, const char *path, float volume) : Sound(soundId),
_volume(volume),
_isLooping(false),
_isPaused(false),
_isMusic(soundId == 1337)
{
    alGenBuffers(1, &buf);
    alGenSources(1, &src);
    buf = alutCreateBufferFromFile(path);

    assert(buf != AL_NONE);

    alSourcei(src, AL_BUFFER, buf);
    alSourcei(src, AL_LOOPING, _isLooping ? AL_TRUE : AL_FALSE);
}

LinuxSound::~LinuxSound()
{
    alDeleteBuffers(1, &buf);
    alDeleteSources(1, &src);
}

void LinuxSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;

    alSourcei(src, AL_LOOPING, _isLooping ? AL_TRUE : AL_FALSE);

    alSourcePlay(src);
}

void LinuxSound::resume()
{
    if (_isPaused)
    {
        alSourcePlay(src);

        _isPaused = false;
    }
}

void LinuxSound::pause()
{
    if (isPlaying())
    {
        alSourcePause(src);

        _isPaused = true;
    }
}

void LinuxSound::stop()
{
    _isLooping = false;
    _isPaused = false;

    alSourceStop(src);
}

void LinuxSound::setVolume(float volume)
{
    _volume = volume;

    alSourcef(src, AL_GAIN, _volume);
}

bool LinuxSound::isLooping()
{
    return _isLooping;
}

bool LinuxSound::isPlaying()
{
    ALint state;
    alGetSourcei(src, AL_SOURCE_STATE, &state);

    return state == AL_PLAYING;
}

bool LinuxSound::isPaused()
{
    return _isPaused;
}
