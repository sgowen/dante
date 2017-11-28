#ifndef _PACKT_SOUND_HPP_
#define _PACKT_SOUND_HPP_

class SoundService;

#include "framework/audio/android/Types.hpp"

class FileData;

class OpenSLESSound
{
public:
    OpenSLESSound(const char* pPath);
    
    const char* getPath();
    
    status load();
    status unload();
    
private:
    friend class SoundService;
    
private:
    const char* mPath;
    FileData* wav_file;
    uint8_t* mBuffer;
    long mLength;
};
#endif
