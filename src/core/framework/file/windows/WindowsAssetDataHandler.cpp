//
//  WindowsAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/file/windows/WindowsAssetDataHandler.h"

#include <stdint.h>
#include <fstream>
#include <assert.h>

WindowsAssetDataHandler* WindowsAssetDataHandler::getInstance()
{
    static WindowsAssetDataHandler instance = WindowsAssetDataHandler();
    return &instance;
}

FileData WindowsAssetDataHandler::getAssetData(const char* relativePath)
{
    assert(relativePath != NULL);
    
    FILE *stream;
    errno_t err;
	assert(fopen_s(&stream, relativePath, "r") == 0);
    
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

void WindowsAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->data != NULL);
    
    free((void *)fileData->data);
}

WindowsAssetDataHandler::WindowsAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
