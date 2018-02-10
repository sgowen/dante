//
//  NGTextureSlot.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTextureSlot__
#define __noctisgames__NGTextureSlot__

#include <framework/graphics/portable/NGGraphics.h>

typedef enum
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    NGTextureSlot_ZERO = GL_TEXTURE0,
    NGTextureSlot_ONE = GL_TEXTURE1
#elif defined _WIN32
    NGTextureSlot_ZERO = 0,
    NGTextureSlot_ONE = 1
#endif
} NGTextureSlot;

#endif /* defined(__noctisgames__NGTextureSlot__) */
