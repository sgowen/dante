//
//  NGTexture.h
//  noctisgames
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
    std::string _name;
    std::string _filePath;
    TextureDataWrapper* _textureDataWrapper;
    TextureWrapper* _textureWrapper;
    TextureManager* _textureManager;
    NGTextureDesc* _desc;
    bool _isLoadingData;
    
    NGTexture(std::string name, NGTextureDesc* desc, TextureManager* textureManager = NULL);
};

#endif /* defined(__noctisgames__NGTexture__) */
