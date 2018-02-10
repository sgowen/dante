//
//  GPUBufferWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/7/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GPUBufferWrapper__
#define __noctisgames__GPUBufferWrapper__

#include <framework/graphics/portable/NGGraphics.h>

struct GPUBufferWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLuint buffer;

    GPUBufferWrapper(GLuint bufferIn) : buffer(bufferIn) {}
#elif defined _WIN32
    ID3D11Buffer* buffer;

    GPUBufferWrapper(ID3D11Buffer* bufferIn) : buffer(bufferIn) {}
#endif
};

#endif /* defined(__noctisgames__GPUBufferWrapper__) */
