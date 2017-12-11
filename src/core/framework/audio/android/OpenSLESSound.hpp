#ifndef _noctisgames_SOUND_HPP_
#define _noctisgames_SOUND_HPP_

#include <cstdlib>

class FileData;

class OpenSLESSound
{
public:
    OpenSLESSound(const char* pPath);
    
    const char* getPath();
    
    bool load();
    bool unload();
    
private:
    friend class SoundService;
    
private:
    const char* mPath;
    FileData* wav_file;
    uint8_t* mBuffer;
    long mLength;
};
#endif
