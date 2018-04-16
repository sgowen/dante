//
//  macros.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__macros__
#define __noctisgames__macros__

#define UNUSED(x) (void)(x)
#define BUFFER_OFFSET(i) ((void*)(i))

#ifdef POSIX
#define ARRAYSIZE(A) ( sizeof(A) / sizeof(A[0]) )
#endif

#define SET_BIT(val, bit, set) val = (val & ~bit) | (set ? bit : 0);

#endif /* defined(__noctisgames__macros__) */
