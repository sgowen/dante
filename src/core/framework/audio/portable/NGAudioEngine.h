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
#include <vector>
#include <string>

class SoundWrapper;
class Sound;
class AudioEngineHelper;

enum AudioEngineState
{
    AudioEngineState_None = 0,
    AudioEngineState_Pause,
    AudioEngineState_Resume
};

enum MusicState
{
    MusicState_None = 0,
    MusicState_Load,
    MusicState_Stop,
    MusicState_Pause,
    MusicState_Resume,
    MusicState_SetVolume,
    MusicState_Play
};

class NGAudioEngine
{
public:
    static void create();
    static NGAudioEngine* getInstance();
    static void destroy();
    
    void update(int flags = 0);
    void render();
    void pause();
    void resume();
    void loadSound(int soundId, const char *path, int numInstances = 1);
    void playSound(int soundId, float inVolume = 1.0f, bool isLooping = false);
    void stopSound(int soundId);
    void pauseSound(int soundId);
    void resumeSound(int soundId);
    void stopAllSounds();
    void pauseAllSounds();
    void resumeAllSounds();
    void loadMusic(const char *path);
    void playMusic(bool isLooping = true, float inVolume = 1.0f);
    void setMusicVolume(float inVolume);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    bool isMusicPlaying();
    bool isMusicLoaded();
    bool isMusicDisabled();
    void setMusicDisabled(bool isMusicDisabled);
    bool areSoundsDisabled();
    void setSoundsDisabled(bool areSoundsDisabled);
    void reset();
    
private:
    static NGAudioEngine* s_instance;
    
    AudioEngineHelper* _audioEngineHelper;
    int _state;
    std::map<int, SoundWrapper*> _sounds;
    std::vector<int> _soundIdsToLoad;
    std::vector<std::string> _soundPathsToLoad;
    std::vector<int> _soundNumInstancesToLoad;
    std::vector<Sound*> _soundsToPlay;
    std::vector<Sound*> _soundsToStop;
    std::vector<Sound*> _soundsToPause;
    std::vector<Sound*> _soundsToResume;
    std::vector<float> _soundsVolumes;
    std::vector<bool> _soundsLooping;
    SoundWrapper* _music;
    std::string _musicPath;
    std::vector<int> _musicStates;
    float _musicVolume;
    bool _isMusicLooping;
    bool _isMusicDisabled;
    bool _areSoundsDisabled;
    
    SoundWrapper* findSound(int soundId);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGAudioEngine();
    ~NGAudioEngine();
    NGAudioEngine(const NGAudioEngine&);
    NGAudioEngine& operator=(const NGAudioEngine&);
};

#endif /* defined(__noctisgames__NGAudioEngine__) */
