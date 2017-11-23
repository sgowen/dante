@@ -1,56 +0,0 @@
//
//  AndroidAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAudioEngineHelper__
#define __noctisgames__AndroidAudioEngineHelper__

#include "framework/audio/portable/AudioEngineHelper.h"

#include <jni.h>

#include <vector>

class SuperpoweredSoundManager;
class SuperpoweredAndroidAudioIO;

class AndroidAudioEngineHelper : public AudioEngineHelper
{
public:
    static AndroidAudioEngineHelper* getInstance();
    
    virtual void update(int flags = 0);
    
    virtual void pause();
    
    virtual void resume();
    
    virtual SoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    
    virtual SoundWrapper* loadMusic(const char* path);
    
    void init(JNIEnv* jni, jobject activity);
    
    void deinit();
    
private:
    JavaVM* _jvm;
    jobject _resources;
    jstring _javaPackageResourcePath;
    const char* _packageResourcePath;
    jstring _packageName;
    SuperpoweredSoundManager* _superpoweredSoundManager;
    std::vector<SuperpoweredAndroidAudioIO*> _audioSystems;
    int _sampleRate;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAudioEngineHelper();
    AndroidAudioEngineHelper(const AndroidAudioEngineHelper&);
    AndroidAudioEngineHelper& operator=(const AndroidAudioEngineHelper&);
};

#endif /* defined(__noctisgames__AndroidAudioEngineHelper__) */

