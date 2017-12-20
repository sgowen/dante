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

FileData WindowsAssetDataHandler::getAssetData(const char* relativePath, bool isText)
{
    if (isText)
    {
        assert(relativePath != NULL);
        
        FILE *stream;
        assert(fopen_s(&stream, relativePath, "r") == 0);
        
        assert(stream != NULL);
        
		// seek to end of file
		fseek(stream, 0, SEEK_END);

		// get current file position which is end from seek
		size_t size = ftell(stream);

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
    }
}

void WindowsAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    assert(fileData != NULL);
	assert(fileData->file_handle != NULL);

	delete fileData->file_handle;
}

WindowsAssetDataHandler::WindowsAssetDataHandler() : AssetDataHandler()
{
    // Empty
}

