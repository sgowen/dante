//
//  TextureWrapper.h
//  noctisgames
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureWrapper__
#define __noctisgames__TextureWrapper__

#include <framework/graphics/portable/NGGraphics.h>

struct TextureWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLuint texture;

    TextureWrapper(GLuint textureIn) : texture(textureIn) {}
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;

    TextureWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
#endif
};

#endif /* defined(__noctisgames__TextureWrapper__) */
