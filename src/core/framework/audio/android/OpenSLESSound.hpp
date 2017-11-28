#ifndef _PACKT_SOUND_HPP_
#define _PACKT_SOUND_HPP_

class SoundService;

#include "framework/audio/android/Resource.hpp"
#include "framework/audio/android/Types.hpp"

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
    Resource mResource;
    uint8_t* mBuffer; off_t mLength;
};
#endif
