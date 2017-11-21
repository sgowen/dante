//
//  LinuxAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/file/linux/LinuxAssetDataHandler.h"

LinuxAssetDataHandler* LinuxAssetDataHandler::getInstance()
{
    static LinuxAssetDataHandler instance = LinuxAssetDataHandler();
    return &instance;
}

FileData LinuxAssetDataHandler::getAssetData(const char* relativePath)
{
    assert(relative_path != NULL);
    
    FILE* stream = fopen(relativePath, "r");
    assert (stream != NULL);
    
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
    assert(file_data != NULL);
    assert(file_data->data != NULL);
    
    free((void *)file_data->data);
}

LinuxAssetDataHandler::LinuxAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
