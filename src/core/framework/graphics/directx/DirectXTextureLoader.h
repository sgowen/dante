//
//  DirectXTextureLoader.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXTextureLoader__
#define __noctisgames__DirectXTextureLoader__

#include <framework/graphics/portable/TextureLoader.h>

class DirectXTextureLoader : public TextureLoader
{
public:
    DirectXTextureLoader();
    
    virtual TextureDataWrapper* loadTextureData(NGTexture* texture);
    virtual TextureWrapper* loadTexture(TextureDataWrapper* textureData, NGTextureDesc* td);
    virtual void destroyTexture(TextureWrapper& textureWrapper);
};

#endif /* defined(__noctisgames__DirectXTextureLoader__) */
