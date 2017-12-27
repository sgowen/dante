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

class NGTexture
{
public:
    std::string name;
    TextureDataWrapper* textureDataWrapper;
    TextureWrapper* textureWrapper;
    TextureManager* _textureManager;
    bool _repeatS;
    bool _isEncrypted;
    bool _isLoadingData;
    
    NGTexture(std::string inName, TextureManager* textureManager, bool repeatS = false, bool isEncrypted = false);
};

#endif /* defined(__noctisgames__NGTexture__) */
