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

FileData LinuxAssetDataHandler::getAssetData(const char* relativePath, bool isText)
{
    assert(relativePath != NULL);

    FILE* stream = fopen(relativePath, "r");
    assert(stream != NULL);

    // seek to end of file
    fseek(stream, 0, SEEK_END);

    // get current file position which is end from seek
    long size = ftell(stream);

    if (isText)
    {
        std::string* rawDataP = new std::string();
        std::string& rawData = *rawDataP;

        // allocate string space and set length

        rawData.resize(size);

        // go back to beginning of file for read
        rewind(stream);

        // read 1*size bytes from sfile into ss
        fread(&rawData[0], 1, size, stream);

        // close the file
        fclose(stream);

        return FileData(size, (void*)rawData.c_str(), rawDataP);
    }
    else
    {
        fseek(stream, 0, SEEK_SET);

        void* buffer = malloc(size);
        fread(buffer, size, 1, stream);

        assert(ferror(stream) == 0);
        fclose(stream);

        return FileData(size, buffer, NULL);
    }
}

void LinuxAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
    assert(fileData->data != NULL);

    if (fileData->file_handle != NULL)
    {
        const std::string* handle = static_cast<const std::string*>(fileData->file_handle);
        delete handle;
    }
    else
    {
        free((void *)fileData->data);
    }
}

LinuxAssetDataHandler::LinuxAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
