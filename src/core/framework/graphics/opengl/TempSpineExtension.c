//
//  TempSpineExtension.c
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "spine/extension.h"

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path)
{
    // Empty
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
    // Empty
}

char* _spUtil_readFile (const char* path, int* length)
{
    return _spReadFile(path, length);
}
