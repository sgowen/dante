//
//  apple_asset_data_handler.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/file/apple/apple_asset_data_handler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#import <Foundation/Foundation.h>
#elif TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
#endif

FileData appleGetAssetData(const char *relative_path)
{
	assert(relative_path != NULL);
    
    const char *bundlePath = getBundlePath(relative_path);
    
    assert(bundlePath != NULL);
    
    FILE* stream = fopen(bundlePath, "r");
    assert(stream != NULL);
    
    // seek to end of file
    fseek(stream, 0, SEEK_END);
    
    // get current file position which is end from seek
    long size = ftell(stream);
    
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

void appleReleaseAssetData(const FileData *file_data)
{
    assert(file_data != NULL);
    assert(file_data->file_handle != NULL);
    
    const std::string* handle = static_cast<const std::string*>(file_data->file_handle);
    delete handle;
}

const char * getPathInsideNSDocuments(const char* relative_path)
{
    NSString *docsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
    NSString *path = [[NSString alloc] initWithCString:relative_path encoding:NSASCIIStringEncoding];
    NSString *filePath = [docsPath stringByAppendingPathComponent:path];

    const char *nsDocumentsPath = [filePath fileSystemRepresentation];
    
    return nsDocumentsPath;
}

const char * getBundlePath(const char* relative_path)
{
    NSString *path = [[NSString alloc] initWithCString:relative_path encoding:NSASCIIStringEncoding];
    const char *bundlePath = [[[NSBundle mainBundle] pathForResource:path ofType:nil] fileSystemRepresentation];
    
    return bundlePath;
}
