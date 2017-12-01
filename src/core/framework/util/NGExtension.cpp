//
//  NGExtension.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include <framework/util/NGExtension.h>

#include <iostream>
#include <assert.h>

namespace NoctisGames
{
    NGExtension* NGExtension::_ngExtension = NULL;
    
    void NGExtension::setInstance(NGExtension* inValue)
    {
        assert(!_ngExtension);
        
        _ngExtension = inValue;
    }
    
    NGExtension* NGExtension::getInstance()
    {
        assert(_ngExtension);
        
        return _ngExtension;
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
