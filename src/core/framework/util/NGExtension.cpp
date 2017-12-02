//
//  NGExtension.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/NGExtension.h>

#include <iostream>
#include <assert.h>

namespace NoctisGames
{
    NGExtension* NGExtension::_instance = NULL;
    
    void NGExtension::setInstance(NGExtension* inValue)
    {
        assert(!_instance);
        
        _instance = inValue;
    }
    
    NGExtension* NGExtension::getInstance()
    {
        assert(_instance);
        
        return _instance;
    }
    
    NGExtension::NGExtension()
    {
        // Empty
    }
    
    NGExtension::~NGExtension()
    {
        // Empty
    }
    
    DefaultNGExtension* DefaultNGExtension::getInstance()
    {
        static DefaultNGExtension ret;
        return &ret;
    }
    
    DefaultNGExtension::DefaultNGExtension() : NGExtension()
    {
        // Empty
    }
    
    DefaultNGExtension::~DefaultNGExtension()
    {
        // Empty
    }
    
    void* DefaultNGExtension::ngAlloc(size_t size, const char* file, int line)
    {
        printf("ng size: %lu, file: %s, line: %d \n", size, file, line);
        
        return malloc(size);
    }
    
    void* DefaultNGExtension::ngCalloc(size_t num, size_t size, const char* file, int line)
    {
        void* ptr = ngAlloc(num * size, file, line);
        if (ptr)
        {
            memset(ptr, 0, num * size);
        }
        
        return ptr;
    }
    
    void* DefaultNGExtension::ngRealloc(void* ptr, size_t size, const char* file, int line)
    {
        printf("ng size: %lu, file: %s, line: %d \n", size, file, line);
        
        return realloc(ptr, size);
    }
    
    void DefaultNGExtension::ngFree(void* mem)
    {
        free(mem);
    }
}
