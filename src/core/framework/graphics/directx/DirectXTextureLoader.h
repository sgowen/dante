//
//  DirectXTextureLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXTextureLoader__
#define __noctisgames__DirectXTextureLoader__

#include "TextureLoader.h"

class DirectXTextureLoader : public TextureLoader
{
public:
    DirectXTextureLoader();
    
    virtual ~DirectXTextureLoader();
    
    virtual GpuTextureDataWrapper* loadTextureData(const char* textureName);
    
    virtual GpuTextureWrapper* loadTexture(GpuTextureDataWrapper* textureData, bool repeatS = false);
};

#endif /* defined(__noctisgames__DirectXTextureLoader__) */
