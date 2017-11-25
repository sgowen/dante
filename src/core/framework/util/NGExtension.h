//
//  Extension.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Extension__
#define __noctisgames__Extension__

/* All allocation uses these. */
#define MALLOC(TYPE,COUNT) ((TYPE*)ngAlloc(sizeof(TYPE) * (COUNT), __FILE__, __LINE__))
#define REALLOC(PTR,TYPE,COUNT) ((TYPE*)ngRealloc(PTR, sizeof(TYPE) * (COUNT), __FILE__, __LINE__))

/* Frees memory. Can be used on const types. */
#define FREE(VALUE) ngFree((void*)VALUE)

/* Call destructor and then frees memory. Can be used on const types. */
#define DESTROY(TYPE,VALUE) VALUE->~TYPE(); ngFree((void*)VALUE)

#include <stdlib.h>

namespace NoctisGames
{
    /// Implement this function to use your own memory allocator.
    void* ngAlloc(size_t size, const char* file, int line);
    
    void* ngRealloc(void* ptr, size_t size, const char* file, int line);
    
    /// If you implement ngAlloc, you should also implement this function.
    void ngFree(void* mem);
}

#endif /* defined(__noctisgames__Extension__) */
