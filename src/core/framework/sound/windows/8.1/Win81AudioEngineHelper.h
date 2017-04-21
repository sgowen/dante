//
//  Win81AudioEngineHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Win81AudioEngineHelper__
#define __noctisgames__Win81AudioEngineHelper__

#define WIN_8_1_AUDIO_ENGINE_HELPER (Win81AudioEngineHelper::getInstance())

#include "IAudioEngineHelper.h"

#include "XAudio2SoundPlayer.h"

#include <memory>

class MediaEnginePlayer;

class Win81AudioEngineHelper : public IAudioEngineHelper
{
public:
	static Win81AudioEngineHelper* getInstance();
    
    virtual void update(int flags = 0);
    
    virtual void pause();
    
    virtual void resume();
    
    virtual ISoundWrapper* loadSound(int soundId, const char *path, int numInstances = 1);
    
    virtual ISoundWrapper* loadMusic(const char* path);

private:
    static XAudio2SoundPlayer * getSoundPlayerInstance();
    
    std::unique_ptr<MediaEnginePlayer> m_mediaPlayer;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Win81AudioEngineHelper();
    virtual ~Win81AudioEngineHelper();
    Win81AudioEngineHelper(const Win81AudioEngineHelper&);
    Win81AudioEngineHelper& operator=(const Win81AudioEngineHelper&);
};

#endif /* defined(__noctisgames__Win81AudioEngineHelper__) */
