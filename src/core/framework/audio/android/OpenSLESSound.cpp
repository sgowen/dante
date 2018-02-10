//
//  OpenSLESSound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/audio/android/OpenSLESSound.h>

#include <framework/audio/android/OpenSLESManager.h>
#include <framework/file/portable/FileData.h>

#include <framework/file/portable/AssetDataHandler.h>

#include <assert.h>

OpenSLESSound::OpenSLESSound(OpenSLESManager* openSLESManager, int soundId, const char *path, float volume) : Sound(soundId),
_openSLESManager(openSLESManager),
_path(path),
_wavFile(NULL),
_buffer(NULL),
_length(0),
_volume(volume),
_isLooping(false),
_isPaused(false),
_isMusic(soundId == 1337)
{
    FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(_path);
    
    _wavFile = new FileData(fileData);
    
    assert(_wavFile && _wavFile->data && _wavFile->file_handle);
    
    _length = _wavFile->data_length;
    _buffer = (uint8_t*) _wavFile->data;
}

OpenSLESSound::~OpenSLESSound()
{
    if (_wavFile)
    {
        AssetDataHandler::getAssetDataHandler()->releaseAssetData(_wavFile);
    }
}

void OpenSLESSound::play(bool isLooping)
{
    _isLooping = isLooping;
    _isPaused = false;
    
    _openSLESManager->playSound(this);
}

void OpenSLESSound::resume()
{
    if (_isPaused)
    {
        _openSLESManager->resumeSound(this);
        
        _isPaused = false;
    }
}

void OpenSLESSound::pause()
{
    if (isPlaying())
    {
        _openSLESManager->pauseSound(this);
        
        _isPaused = true;
    }
}

void OpenSLESSound::stop()
{
    _isLooping = false;
    _isPaused = false;
    
    _openSLESManager->stopSound(this);
}

void OpenSLESSound::setVolume(float volume)
{
    _volume = volume;
    
    _openSLESManager->setVolume(this);
}

bool OpenSLESSound::isLooping()
{
    return _isLooping;
}

bool OpenSLESSound::isPlaying()
{
    return _openSLESManager->isPlaying(this);
}

bool OpenSLESSound::isPaused()
{
    return _isPaused;
}
