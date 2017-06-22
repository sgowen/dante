//
//  macros.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__macros__
#define __noctisgames__macros__

#define NG_M_PI 3.1415926535f

#define UNUSED(x) (void)(x)

#define DEGREES_TO_RADIANS(angle) ( (angle) / 180.0f * NG_M_PI )
#define RADIANS_TO_DEGREES(angle) ( (angle) / NG_M_PI * 180.0f )

#ifdef POSIX
#define ARRAYSIZE(A) ( sizeof(A) / sizeof(A[0]) )
#endif

#endif /* defined(__noctisgames__macros__) */
