//
//  pch.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__pch__
#define __noctisgames__pch__

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_OSX
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#endif

#endif /* defined(__noctisgames__pch__) */
