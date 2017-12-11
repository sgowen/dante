//
//  OpenSLESManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenSLESManager__
#define __noctisgames__OpenSLESManager__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include <vector>

#define MAX_NUM_CONCURRENT_SOUNDS 12

class OpenSLESSound;

class OpenSLESManager
{
public:
    static void create();
    
    static void destroy();
    
    static OpenSLESManager* getInstance();
    
    OpenSLESSound* _activeSounds[MAX_NUM_CONCURRENT_SOUNDS];
    
    void playSound(OpenSLESSound* sound);
    void resumeSound(OpenSLESSound* sound);
    void pauseSound(OpenSLESSound* sound);
    void stopSound(OpenSLESSound* sound);
    void setVolume(OpenSLESSound* sound);
    bool isPlaying(OpenSLESSound* sound);
    
    int getNextBufferIndex();
    
    int getBufferIndexOfSound(OpenSLESSound* sound);
    
private:
    static OpenSLESManager* _instance;
    
    // OpenSL ES engine.
    SLObjectItf _engineObj;
    SLEngineItf _engine;
    // Audio output.
    SLObjectItf _outputMixObj;
    
    SLObjectItf _playerObjs[MAX_NUM_CONCURRENT_SOUNDS];
    SLPlayItf _players[MAX_NUM_CONCURRENT_SOUNDS];
    SLAndroidSimpleBufferQueueItf _playerQueues[MAX_NUM_CONCURRENT_SOUNDS];
    SLVolumeItf _playerVolumes[MAX_NUM_CONCURRENT_SOUNDS];
    
    int _nextBufferIndex;
    
    void initBuffers();
    
    void deinitBuffers();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    OpenSLESManager();
    ~OpenSLESManager();
    OpenSLESManager(const OpenSLESManager&);
    OpenSLESManager& operator=(const OpenSLESManager&);
};

#endif /* defined(__noctisgames__OpenSLESManager__) */
