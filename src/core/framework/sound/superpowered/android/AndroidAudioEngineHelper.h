//
//  AndroidAudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAudioEngineHelper__
#define __noctisgames__AndroidAudioEngineHelper__

#define ANDROID_AUDIO_ENGINE_HELPER (AndroidAudioEngineHelper::getInstance())

#include "IAudioEngineHelper.h"

#include <jni.h>

#include <vector>

class SuperpoweredSoundManager;
class SuperpoweredAndroidAudioIO;

class AndroidAudioEngineHelper : public IAudioEngineHelper
{
public:
	static AndroidAudioEngineHelper* getInstance();
    
    virtual void update(int flags = 0);
    
    virtual void pause();
    
    virtual void resume();
    
    virtual ISoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    
    virtual ISoundWrapper* loadMusic(const char* path);
    
    void init(JNIEnv* jni, jobject activity);
    
    void deinit();

private:
    JavaVM* m_jvm;
    jobject m_resources;
    jstring m_javaPackageResourcePath;
    const char* m_packageResourcePath;
    jstring m_packageName;
    SuperpoweredSoundManager* m_superpoweredSoundManager;
    std::vector<SuperpoweredAndroidAudioIO*> m_audioSystems;
    int m_iSampleRate;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAudioEngineHelper();
    AndroidAudioEngineHelper(const AndroidAudioEngineHelper&);
    AndroidAudioEngineHelper& operator=(const AndroidAudioEngineHelper&);
};

#endif /* defined(__noctisgames__AndroidAudioEngineHelper__) */
