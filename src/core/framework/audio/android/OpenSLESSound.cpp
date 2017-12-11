#include "framework/audio/android/OpenSLESSound.hpp"

#include "framework/file/portable/FileData.h"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/util/StringUtil.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

OpenSLESSound::OpenSLESSound(const char* pPath) :
mPath(pPath),
wav_file(NULL)
{
    // Empty
}

const char* OpenSLESSound::getPath()
{
    return mPath;
}

bool OpenSLESSound::load()
{
    LOG("Loading sound %s", mPath);
    
    FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(mPath);
    
    wav_file = new FileData(fileData);
    
    if (wav_file
        && wav_file->data
        && wav_file->file_handle)
    {
        mLength = wav_file->data_length;
        mBuffer = (uint8_t*) wav_file->data;
        
        return true;
    }
    else
    {
        LOG("Error while reading PCM sound.");
        return false;
    }
}

bool OpenSLESSound::unload()
{
    if (wav_file)
    {
        AssetDataHandler::getAssetDataHandler()->releaseAssetData(wav_file);
        
        return true;
    }
    
    return false;
}
