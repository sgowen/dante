//
//  TextureLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureLoader__
#define __noctisgames__TextureLoader__

struct GpuTextureDataWrapper;
struct GpuTextureWrapper;

class TextureLoader
{
public:
    TextureLoader();
    
    virtual ~TextureLoader();
    
    virtual GpuTextureDataWrapper* loadTextureData(const char* textureName) = 0;
    
    virtual GpuTextureWrapper* loadTexture(GpuTextureDataWrapper* textureData, bool repeatS = false) = 0;
};

#endif /* defined(__noctisgames__TextureLoader__) */
