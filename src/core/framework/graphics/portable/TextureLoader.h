//
//  TextureLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureLoader__
#define __noctisgames__TextureLoader__

struct TextureDataWrapper;
class NGTexture;
struct TextureWrapper;
struct NGTextureDesc;

class TextureLoader
{
public:
    TextureLoader();
    virtual ~TextureLoader();
    
    virtual TextureDataWrapper* loadTextureData(NGTexture* texture) = 0;
    virtual TextureWrapper* loadTexture(TextureDataWrapper* textureData, NGTextureDesc* td) = 0;
    virtual void destroyTexture(TextureWrapper& textureWrapper) = 0;
};

#endif /* defined(__noctisgames__TextureLoader__) */
