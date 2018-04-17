//
//  OpenSLESSound.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenSLESSound__
#define __noctisgames__OpenSLESSound__

#include <framework/audio/portable/Sound.h>

#include <cstdlib>

class OpenSLESManager;
class FileData;

class OpenSLESSound : public Sound
{
public:
    OpenSLESManager* _openSLESManager;
    const char* _path;
    FileData* _wavFile;
    uint8_t* _buffer;
    long _length;
    float _volume;
    bool _isLooping;
    bool _isPaused;
    bool _isMusic;
    
    OpenSLESSound(OpenSLESManager* openSLESManager, int soundId, const char *path, float volume = 1.0f);
    
    virtual ~OpenSLESSound();

    virtual void play(bool isLooping = false);
    
    virtual void resume();
    
    virtual void pause();
    
    virtual void stop();
    
    virtual void setVolume(float volume);
    
    virtual bool isLooping();
    
    virtual bool isPlaying();
    
    virtual bool isPaused();
};

#endif /* defined(__noctisgames__OpenSLESSound__) */
