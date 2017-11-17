//
//  Direct3DTextureLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DTextureLoader__
#define __noctisgames__Direct3DTextureLoader__

#include "TextureLoader.h"

class Direct3DTextureLoader : public TextureLoader
{
public:
    Direct3DTextureLoader();
    
    virtual ~Direct3DTextureLoader();
    
    virtual GpuTextureDataWrapper* loadTextureData(const char* textureName);
    
    virtual GpuTextureWrapper* loadTexture(GpuTextureDataWrapper* textureData, bool repeatS = false);
};

#endif /* defined(__noctisgames__Direct3DTextureLoader__) */
