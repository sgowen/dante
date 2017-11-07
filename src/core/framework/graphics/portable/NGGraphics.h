//
//  NGGraphics.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGGraphics_h__
#define __noctisgames__NGGraphics_h__

#if defined __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_OS_OSX
        #import <OpenGL/OpenGL.h>
        #import <OpenGL/gl.h>
    #endif
#elif defined __ANDROID__
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#elif defined _WIN32
    #include <d3d11_1.h>
#endif

#endif /* __noctisgames__NGGraphics_h__ */
