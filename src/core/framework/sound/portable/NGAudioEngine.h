//
//  NGAudioEngine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGAudioEngine__
#define __noctisgames__NGAudioEngine__

#define NG_AUDIO_ENGINE (NGAudioEngine::getInstance())

#include <map>

class ISoundWrapper;
class IAudioEngineHelper;

class NGAudioEngine
{
public:
    static NGAudioEngine* getInstance();
    
    void update(int flags = 0);
    
    void pause();
    
    void resume();
    
    void loadSound(int soundId, const char *path, int numInstances = 1);
    
    void playSound(int soundId, bool isLooping = false, float volume = 1.0f);
    
    void stopSound(int soundId);
    
    void pauseSound(int soundId);
    
    void resumeSound(int soundId);
    
    void stopAllSounds();
    
    void pauseAllSounds();
    
    void resumeAllSounds();
    
    void resetSounds();
    
    void loadMusic(const char *path);
    
    void playMusic(bool isLooping = true, float volume = 1.0f);
    
    void setMusicVolume(float volume);
    
    void stopMusic();
    
    void pauseMusic();
    
    void resumeMusic();
    
    bool isMusicLoaded();
    
    bool isMusicDisabled();
    
    void setMusicDisabled(bool isMusicDisabled);
    
    bool isSoundDisabled();
    
    void setSoundDisabled(bool isSoundDisabled);
    
private:
    std::map<int, ISoundWrapper*> m_sounds;
    ISoundWrapper* m_music;
    IAudioEngineHelper* m_audioEngineHelper;
    int m_iNumSoundsPlayedThisFrame;
    bool m_isMusicDisabled;
    bool m_isSoundDisabled;
    
    ISoundWrapper* findSound(int soundId);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGAudioEngine();
    ~NGAudioEngine();
    NGAudioEngine(const NGAudioEngine&);
    NGAudioEngine& operator=(const NGAudioEngine&);
};

#endif /* defined(__noctisgames__NGAudioEngine__) */
