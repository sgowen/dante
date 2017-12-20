//
//  LinuxAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/file/linux/LinuxAssetDataHandler.h"

#include <stdint.h>
#include <fstream>
#include <assert.h>

LinuxAssetDataHandler* LinuxAssetDataHandler::getInstance()
{
    static LinuxAssetDataHandler instance = LinuxAssetDataHandler();
    return &instance;
}

FileData LinuxAssetDataHandler::getAssetData(const char* relativePath)
{
    assert(relativePath != NULL);

    FILE* stream = fopen(relativePath, "r");
    assert(stream != NULL);

    fseek(stream, 0, SEEK_END);
    long stream_size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    void* buffer = malloc(stream_size);
    fread(buffer, stream_size, 1, stream);

    assert(ferror(stream) == 0);
    fclose(stream);

    return FileData(stream_size, buffer, NULL);
}

void LinuxAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->data != NULL);

    free((void *)fileData->data);
}

LinuxAssetDataHandler::LinuxAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
