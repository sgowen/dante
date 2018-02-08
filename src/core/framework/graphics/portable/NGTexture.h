//
//  NGTexture.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTexture__
#define __noctisgames__NGTexture__

#include <string>

struct TextureDataWrapper;
struct TextureWrapper;
class TextureManager;
struct NGTextureDesc;

class NGTexture
{
public:
    std::string name;
    std::string filePath;
    TextureDataWrapper* textureDataWrapper;
    TextureWrapper* textureWrapper;
    TextureManager* _textureManager;
    NGTextureDesc* _desc;
    bool _isFramebuffer;
    bool _isLoadingData;
    
    NGTexture(std::string inName, TextureManager* textureManager, NGTextureDesc* desc);
};

#endif /* defined(__noctisgames__NGTexture__) */
