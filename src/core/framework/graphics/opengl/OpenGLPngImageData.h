//
//  OpenGLPngImageData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLPngImageData__
#define __noctisgames__OpenGLPngImageData__

#include "framework/graphics/portable/NGGraphics.h"

struct OpenGLPngImageData
{
    const int width;
    const int height;
    const int size;
    const GLenum gl_color_format;
    const void* data;
};

#endif /* defined(__noctisgames__OpenGLPngImageData__) */
