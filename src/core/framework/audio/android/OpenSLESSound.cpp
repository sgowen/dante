#include "framework/audio/android/OpenSLESSound.hpp"
#include "framework/audio/android/Log.hpp"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"

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

status OpenSLESSound::load()
{
    Log::info("Loading sound %s", mPath);
    
    FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(mPath);
    
    wav_file = new FileData(fileData);
    
    if (wav_file
        && wav_file->data
        && wav_file->file_handle)
    {
        mLength = wav_file->data_length;
        mBuffer = (uint8_t*) wav_file->data;
        
        return STATUS_OK;
    }
    else
    {
        Log::error("Error while reading PCM sound.");
        return STATUS_KO;
    }
}

status OpenSLESSound::unload()
{
    if (wav_file)
    {
        AssetDataHandler::getAssetDataHandler()->releaseAssetData(wav_file);
        
        return STATUS_OK;
    }
    
    return STATUS_KO;
}
