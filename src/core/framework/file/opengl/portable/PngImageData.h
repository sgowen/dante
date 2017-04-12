//
//  PngImageData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PngImageData__
#define __noctisgames__PngImageData__

#include "platform_gl.h"

struct PngImageData
{
    const int width;
    const int height;
    const int size;
    const GLenum gl_color_format;
    const void* data;
};

#endif /* defined(__noctisgames__PngImageData__) */
