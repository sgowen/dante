#include "framework/audio/android/SoundService.hpp"

#include <string>
#include <math.h>
#include <assert.h>
#include "framework/util/StringUtil.h"

SoundService::SoundService() :
mEngineObj(NULL),
mEngine(NULL),
mOutputMixObj(NULL),
mPlayerObj(NULL), mPlayer(NULL), mPlayerQueue(NULL), mPlayerVolume(NULL),
mSoundCount(0)
{
    LOG("Creating SoundService.");
}

SoundService::~SoundService()
{
    LOG("Destroying SoundService.");
    
    for (int32_t i = 0; i < mSoundCount; ++i)
    {
        delete mSounds[i];
        mSoundCount = 0;
    }
}

void SoundService::start()
{
    SLresult lRes;
    const SLuint32      lEngineMixIIDCount = 1;
    const SLInterfaceID lEngineMixIIDs[]   = {SL_IID_ENGINE};
    const SLboolean     lEngineMixReqs[]   = {SL_BOOLEAN_TRUE};
    const SLuint32      lOutputMixIIDCount = 0;
    const SLInterfaceID lOutputMixIIDs[]   = {};
    const SLboolean     lOutputMixReqs[]   = {};
    
    // Creates OpenSL ES engine and dumps its capabilities.
    lRes = slCreateEngine(&mEngineObj, 0, NULL,
                          lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
    assert(lRes == SL_RESULT_SUCCESS);
    lRes = (*mEngineObj)->Realize(mEngineObj,SL_BOOLEAN_FALSE);
    assert(lRes == SL_RESULT_SUCCESS);
    lRes = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE,
                                       &mEngine);
    assert(lRes == SL_RESULT_SUCCESS);
    
    // Creates audio output.
    lRes = (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj,
                                       lOutputMixIIDCount, lOutputMixIIDs, lOutputMixReqs);
    assert(lRes == SL_RESULT_SUCCESS);
    lRes = (*mOutputMixObj)->Realize(mOutputMixObj,
                                     SL_BOOLEAN_FALSE);
    assert(lRes == SL_RESULT_SUCCESS);
    
    // Set-up sound player.
    startSoundPlayer();
    
    // Loads resources
    for (int32_t i = 0; i < mSoundCount; ++i)
    {
        assert(mSounds[i]->load());
    }
}

void SoundService::stop()
{
    LOG("Stopping SoundService.");
    
    // Destroys sound player.
    if (mPlayerObj != NULL)
    {
        (*mPlayerObj)->Destroy(mPlayerObj);
        mPlayerObj = NULL; mPlayer = NULL; mPlayerQueue = NULL; mPlayerVolume = NULL;
    }
    
    // Destroys audio output and engine.
    if (mOutputMixObj != NULL)
    {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = NULL;
    }
    
    if (mEngineObj != NULL)
    {
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = NULL; mEngine = NULL;
    }
    
    // Frees sound resources.
    for (int32_t i = 0; i < mSoundCount; ++i)
    {
        mSounds[i]->unload();
    }
}

void SoundService::startSoundPlayer()
{
    LOG("Starting sound player.");
    SLresult lRes;
    
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
    lDataLocatorOut.outputMix = mOutputMixObj;
    
    SLDataSink lDataSink;
    lDataSink.pLocator = &lDataLocatorOut;
    lDataSink.pFormat = NULL;
    
    // Creates the sounds player and retrieves its interfaces.
    const SLuint32 lSoundPlayerIIDCount = 3;
    const SLInterfaceID lSoundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
    const SLboolean lSoundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    
    lRes = (*mEngine)->CreateAudioPlayer(mEngine, &mPlayerObj, &lDataSource, &lDataSink, lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs);
    assert(lRes == SL_RESULT_SUCCESS);
    
    lRes = (*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE);
    assert(lRes == SL_RESULT_SUCCESS);
    
    lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayer);
    assert(lRes == SL_RESULT_SUCCESS);
    
    lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerQueue);
    assert(lRes == SL_RESULT_SUCCESS);
    
    lRes = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_VOLUME, &mPlayerVolume);
    assert(lRes == SL_RESULT_SUCCESS);
    
    // Starts the sound player. Nothing can be heard while the
    // sound queue remains empty.
    lRes = (*mPlayer)->SetPlayState(mPlayer, SL_PLAYSTATE_PLAYING);
    assert(lRes == SL_RESULT_SUCCESS);
}

OpenSLESSound* SoundService::registerSound(const char* pPath)
{
    OpenSLESSound* lSound = new OpenSLESSound(pPath);
    mSounds[mSoundCount++] = lSound;
    return lSound;
}

double log2(double n)
{
    return log(n) / log(2.0);
}

void SoundService::playSound(OpenSLESSound* pSound)
{
    SLresult lRes;
    SLuint32 lPlayerState;
    (*mPlayerObj)->GetState(mPlayerObj, &lPlayerState);
    if (lPlayerState == SL_OBJECT_STATE_REALIZED)
    {
        int16_t* lBuffer = (int16_t*) pSound->mBuffer;
        off_t lLength = pSound->mLength;
        
        // Removes any sound from the queue.
        lRes = (*mPlayerQueue)->Clear(mPlayerQueue);
        assert(lRes == SL_RESULT_SUCCESS);
        
        // Plays the new sound.
        lRes = (*mPlayerQueue)->Enqueue(mPlayerQueue, lBuffer, lLength);
        assert(lRes == SL_RESULT_SUCCESS);
        
        // Sets Volume
        float x = 0.1f;

        int dBVolume = (int) (20 * log2(x) / log2(10));
        SLmillibel volume = (SLmillibel) (dBVolume * 100); //1dB = 100mB

        //set
        lRes = (*mPlayerVolume)->SetVolumeLevel(mPlayerVolume, volume);
        assert(lRes == SL_RESULT_SUCCESS);
    }
}
