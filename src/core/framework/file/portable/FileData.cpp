//
//  FileData.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/file/portable/FileData.h>

FileData::FileData(long data_length_in, void *data_in, void *file_handle_in) :
data_length(data_length_in),
data(data_in),
file_handle(file_handle_in)
{
	// Empty
}

FileData::FileData(const FileData& inFileData) :
data_length(inFileData.data_length),
data(inFileData.data),
file_handle(inFileData.file_handle)
{
    // Empty
}
