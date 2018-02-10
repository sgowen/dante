//
//  WindowsAssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/file/windows/WindowsAssetDataHandler.h>

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
	assert(relativePath != NULL);

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

void WindowsAssetDataHandler::releaseAssetData(const FileData* fileData)
{
	assert(fileData != NULL);
	assert(fileData->file_handle != NULL);

    const std::vector<uint8_t>* handle = static_cast<const std::vector<uint8_t>*>(fileData->file_handle);
    delete handle;
}

WindowsAssetDataHandler::WindowsAssetDataHandler() : AssetDataHandler()
{
	// Empty
}
