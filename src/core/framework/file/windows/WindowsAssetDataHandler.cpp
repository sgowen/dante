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
#include <vector>
#include <assert.h>

WindowsAssetDataHandler* WindowsAssetDataHandler::getInstance()
{
    static WindowsAssetDataHandler instance = WindowsAssetDataHandler();
    return &instance;
}

FileData WindowsAssetDataHandler::getAssetData(const char* relativePath)
{
    wchar_t* name = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, relativePath, -1, name, 4096);
    
    std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (!inFile)
    {
        wchar_t moduleName[_MAX_PATH];
        
        assert(GetModuleFileNameW(NULL, moduleName, _MAX_PATH));
        
        wchar_t drive[_MAX_DRIVE];
        wchar_t path[_MAX_PATH];
        
        assert(!_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, NULL, 0, NULL, 0));
        
        wchar_t filename[_MAX_PATH];
        assert(!_wmakepath_s(filename, _MAX_PATH, drive, path, name, NULL));
        
        inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
    }
    
    assert(inFile);
    
    std::streampos len = inFile.tellg();
    assert(inFile);
    
    std::vector<uint8_t>* blob = new std::vector<uint8_t>();
    blob->resize(size_t(len));
    
    inFile.seekg(0, std::ios::beg);
    assert(inFile);
    
    inFile.read(reinterpret_cast<char*>(blob->data()), len);
    assert(inFile);
    
    inFile.close();
    
    delete name;
    
    return FileData(blob->size(), blob->data(), blob);
    
//    assert(relativePath != NULL);
//    
//    FILE *stream;
//    errno_t err;
//    assert(fopen_s(&stream, relativePath, "r") == 0);
//    
//    assert(stream != NULL);
//    
//    fseek(stream, 0, SEEK_END);
//    long stream_size = ftell(stream);
//    fseek(stream, 0, SEEK_SET);
//    
//    void* buffer = malloc(stream_size);
//    fread(buffer, stream_size, 1, stream);
//    
//    assert(ferror(stream) == 0);
//    fclose(stream);
//    
//    return FileData(stream_size, buffer, NULL);
}

void WindowsAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->file_handle != NULL);
    
    delete fileData->file_handle;
    
//    assert(fileData != NULL);
//    assert(fileData->data != NULL);
//
//    free((void *)fileData->data);
}

WindowsAssetDataHandler::WindowsAssetDataHandler() : AssetDataHandler()
{
    // Empty
}

