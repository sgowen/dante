//
//  Extension.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/util/Extension.h>

//#include <iostream>
#include <fstream>

namespace NoctisGames
{
    void* ngAlloc(size_t size, const char* file, int line)
    {
        //printf("ngAlloc size: %lu, file: %s, line: %d \n", size, file, line);
        
        return malloc(size);
    }
    
    void* ngRealloc(void* ptr, size_t size, const char* file, int line)
    {
        //printf("ngRealloc size: %lu, file: %s, line: %d \n", size, file, line);
        
        return realloc(ptr, size);
    }
    
    void ngFree(void* mem)
    {
        free(mem);
    }
}
