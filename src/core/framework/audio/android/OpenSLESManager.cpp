//
//  OpenSLESManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/audio/android/OpenSLESManager.h>

#include <framework/audio/android/OpenSLESSound.h>

#include <framework/util/MathUtil.h>

#include <assert.h>

OpenSLESManager* OpenSLESManager::_instance = NULL;

void OpenSLESManager::create()
{
    assert(!_instance);
    
    _instance = new OpenSLESManager();
}

void OpenSLESManager::destroy()
{
    assert(_instance);
    
    delete _instance;
    _instance = NULL;
}

OpenSLESManager * OpenSLESManager::getInstance()
{
    return _instance;
}

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(context != NULL);
    
    OpenSLESSound* sound = static_cast<OpenSLESSound*>(context);
    
    if (sound->_isLooping)
    {
        SLresult result;
        
        int16_t* lBuffer = (int16_t*) sound->_buffer;
        off_t lLength = sound->_length;
        
        // Plays the new sound.
        result = (*bq)->Enqueue(bq, lBuffer, lLength);
        assert(result == SL_RESULT_SUCCESS);
    }
    else
    {
        sound->_isPaused = false;
        sound->_isLooping = false;
        int i = sound->_openSLESManager->getBufferIndexOfSound(sound);
        if (i >= 0)
        {
            sound->_openSLESManager->_activeSounds[i] = NULL;
        }
    }
}

double log2(double n)
{
    return log(n) / log(2.0);
}

void OpenSLESManager::playSound(OpenSLESSound* sound)
{
    int i = getBufferIndexOfSound(sound);
    if (i < 0)
    {
        i = getNextBufferIndex();
    }
    
    assert(i >= 0);
    
    SLresult result;
    SLuint32 lPlayerState;
    (*_playerObjs[i])->GetState(_playerObjs[i], &lPlayerState);
    if (lPlayerState == SL_OBJECT_STATE_REALIZED)
    {
        int16_t* lBuffer = (int16_t*) sound->_buffer;
        off_t lLength = sound->_length;
        
        // Removes any sound from the queue.
        result = (*_playerQueues[i])->Clear(_playerQueues[i]);
        assert(result == SL_RESULT_SUCCESS);
        
        result = (*_players[i])->SetPlayState(_players[i], SL_PLAYSTATE_STOPPED);
        assert(result == SL_RESULT_SUCCESS);
        
        // Plays the new sound.
        result = (*_playerQueues[i])->Enqueue(_playerQueues[i], lBuffer, lLength);
        assert(result == SL_RESULT_SUCCESS);
        
        // Sets Volume
        float x = sound->_volume;
        x = clamp(x, 0.0f, 1.0f);
        
        int dBVolume = (int) (20 * log2(x) / log2(10));
        SLmillibel volume = (SLmillibel) (dBVolume * 100); //1dB = 100mB
        
        result = (*_playerVolumes[i])->SetVolumeLevel(_playerVolumes[i], volume);
        assert(result == SL_RESULT_SUCCESS);
        
        // register callback on the buffer queue
        result = (*_playerQueues[i])->RegisterCallback(_playerQueues[i], bqPlayerCallback, (void*)sound);
        assert(result == SL_RESULT_SUCCESS);
        
        // Starts the sound player. Nothing can be heard while the
        // sound queue remains empty.
        result = (*_players[i])->SetPlayState(_players[i], SL_PLAYSTATE_PLAYING);
        assert(result == SL_RESULT_SUCCESS);
        
        _activeSounds[i] = sound;
    }
}

void OpenSLESManager::resumeSound(OpenSLESSound* sound)
{
    int i = getBufferIndexOfSound(sound);
    
    if (i >= 0)
    {
        SLresult result = (*_players[i])->SetPlayState(_players[i], SL_PLAYSTATE_PLAYING);
        assert(result == SL_RESULT_SUCCESS);
    }
}

void OpenSLESManager::pauseSound(OpenSLESSound* sound)
{
    int i = getBufferIndexOfSound(sound);
    
    if (i >= 0)
    {
        SLresult result = (*_players[i])->SetPlayState(_players[i], SL_PLAYSTATE_PAUSED);
        assert(result == SL_RESULT_SUCCESS);
    }
}

void OpenSLESManager::stopSound(OpenSLESSound* sound)
{
    int i = getBufferIndexOfSound(sound);
    
    if (i >= 0)
    {
        SLresult result = (*_players[i])->SetPlayState(_players[i], SL_PLAYSTATE_STOPPED);
        assert(result == SL_RESULT_SUCCESS);
        
        _activeSounds[i] = NULL;
    }
}

void OpenSLESManager::setVolume(OpenSLESSound* sound)
{
    int i = getBufferIndexOfSound(sound);
    
    if (i >= 0)
    {
        // Sets Volume
        float x = sound->_volume;
        x = clamp(x, 0.0f, 1.0f);
        
        int dBVolume = (int) (20 * log2(x) / log2(10));
        SLmillibel volume = (SLmillibel) (dBVolume * 100); //1dB = 100mB
        
        SLresult result = (*_playerVolumes[i])->SetVolumeLevel(_playerVolumes[i], volume);
        assert(result == SL_RESULT_SUCCESS);
    }
}

bool OpenSLESManager::isPlaying(OpenSLESSound* sound)
{
    return getBufferIndexOfSound(sound) >= 0;
}

int OpenSLESManager::getNextBufferIndex()
{
    for (int i = 0; i < MAX_NUM_CONCURRENT_SOUNDS; ++i)
    {
        _nextBufferIndex++;
        if (_nextBufferIndex >= MAX_NUM_CONCURRENT_SOUNDS)
        {
            _nextBufferIndex = 0;
        }
        
        if (_activeSounds[_nextBufferIndex] == NULL
            || (!_activeSounds[_nextBufferIndex]->_isLooping
                && !_activeSounds[_nextBufferIndex]->_isMusic))
        {
            return _nextBufferIndex;
        }
    }
    
    return -1;
}

int OpenSLESManager::getBufferIndexOfSound(OpenSLESSound* sound)
{
    for (int i = 0; i < MAX_NUM_CONCURRENT_SOUNDS; ++i)
    {
        if (_activeSounds[i] == sound)
        {
            return i;
        }
    }
    
    return -1;
}

void OpenSLESManager::initBuffers()
{
    for (int i = 0; i < MAX_NUM_CONCURRENT_SOUNDS; ++i)
    {
        SLresult result;
        
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue lDataLocatorIn;
        lDataLocatorIn.locatorType =
        SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        // At most one buffer in the queue.
        lDataLocatorIn.numBuffers = 1;
        
        SLDataFormat_PCM lDataFormat;
        lDataFormat.formatType = SL_DATAFORMAT_PCM;
        lDataFormat.numChannels = 1; // Mono sound.
        lDataFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
        lDataFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        lDataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        lDataFormat.channelMask = SL_SPEAKER_FRONT_CENTER;
        lDataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
        
        SLDataSource lDataSource;
        lDataSource.pLocator = &lDataLocatorIn;
        lDataSource.pFormat = &lDataFormat;
        
        SLDataLocator_OutputMix lDataLocatorOut;
        lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
        lDataLocatorOut.outputMix = _outputMixObj;
        
        SLDataSink lDataSink;
        lDataSink.pLocator = &lDataLocatorOut;
        lDataSink.pFormat = NULL;
        
        // Creates the sounds player and retrieves its interfaces.
        const SLuint32 lSoundPlayerIIDCount = 3;
        const SLInterfaceID lSoundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
        const SLboolean lSoundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        
        result = (*_engine)->CreateAudioPlayer(_engine, &_playerObjs[i], &lDataSource, &lDataSink, lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs);
        assert(result == SL_RESULT_SUCCESS);
        
        result = (*_playerObjs[i])->Realize(_playerObjs[i], SL_BOOLEAN_FALSE);
        assert(result == SL_RESULT_SUCCESS);
        
        result = (*_playerObjs[i])->GetInterface(_playerObjs[i], SL_IID_PLAY, &_players[i]);
        assert(result == SL_RESULT_SUCCESS);
        
        result = (*_playerObjs[i])->GetInterface(_playerObjs[i], SL_IID_BUFFERQUEUE, &_playerQueues[i]);
        assert(result == SL_RESULT_SUCCESS);
        
        result = (*_playerObjs[i])->GetInterface(_playerObjs[i], SL_IID_VOLUME, &_playerVolumes[i]);
        assert(result == SL_RESULT_SUCCESS);
    }
}

void OpenSLESManager::deinitBuffers()
{
    for (int i = 0; i < MAX_NUM_CONCURRENT_SOUNDS; ++i)
    {
        if (_playerObjs[i] != NULL)
        {
            (*_playerObjs[i])->Destroy(_playerObjs[i]);
            _playerObjs[i] = NULL;
            _players[i] = NULL;
            _playerQueues[i] = NULL;
            _playerVolumes[i] = NULL;
        }
    }
}

OpenSLESManager::OpenSLESManager() :
_engineObj(NULL),
_engine(NULL),
_outputMixObj(NULL),
_nextBufferIndex(0)
{
    for (int i = 0; i < MAX_NUM_CONCURRENT_SOUNDS; ++i)
    {
        _playerObjs[i] = NULL;
        _players[i] = NULL;
        _playerQueues[i] = NULL;
        _playerVolumes[i] = NULL;
        
        _activeSounds[i] = NULL;
    }
    
    SLresult result;
    const SLuint32      lEngineMixIIDCount = 1;
    const SLInterfaceID lEngineMixIIDs[]   = {SL_IID_ENGINE};
    const SLboolean     lEngineMixReqs[]   = {SL_BOOLEAN_TRUE};
    const SLuint32      lOutputMixIIDCount = 0;
    const SLInterfaceID lOutputMixIIDs[]   = {};
    const SLboolean     lOutputMixReqs[]   = {};
    
    result = slCreateEngine(&_engineObj, 0, NULL, lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
    assert(result == SL_RESULT_SUCCESS);
    
    result = (*_engineObj)->Realize(_engineObj, SL_BOOLEAN_FALSE);
    assert(result == SL_RESULT_SUCCESS);
    
    result = (*_engineObj)->GetInterface(_engineObj, SL_IID_ENGINE, &_engine);
    assert(result == SL_RESULT_SUCCESS);
    
    result = (*_engine)->CreateOutputMix(_engine, &_outputMixObj, lOutputMixIIDCount, lOutputMixIIDs, lOutputMixReqs);
    assert(result == SL_RESULT_SUCCESS);
    
    result = (*_outputMixObj)->Realize(_outputMixObj, SL_BOOLEAN_FALSE);
    assert(result == SL_RESULT_SUCCESS);
    
    initBuffers();
}

OpenSLESManager::~OpenSLESManager()
{
    deinitBuffers();
    
    // Destroys audio output and engine.
    if (_outputMixObj != NULL)
    {
        (*_outputMixObj)->Destroy(_outputMixObj);
        _outputMixObj = NULL;
    }
    
    if (_engineObj != NULL)
    {
        (*_engineObj)->Destroy(_engineObj);
        _engineObj = NULL;
        _engine = NULL;
    }
}
