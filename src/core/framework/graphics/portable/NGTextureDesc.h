//
//  NGTextureDesc.h
//  noctisgames
//
//  Created by Stephen Gowen on 12/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTextureDesc__
#define __noctisgames__NGTextureDesc__

#include <string>

struct NGTextureDesc
{
    std::string _textureName;
    int _layer;
    bool _repeatS;
    bool _isEncrypted;
    bool _hasNormal;
    std::string _textureFilterMin;
    std::string _textureFilterMag;
    bool _textureFilterMipMap;
    
    NGTextureDesc(std::string textureName, int layer, bool repeatS, bool isEncrypted, bool hasNormal, std::string textureFilterMin, std::string textureFilterMag, bool textureFilterMipMap) :
    _textureName(textureName),
    _layer(layer),
    _repeatS(repeatS),
    _isEncrypted(isEncrypted),
    _hasNormal(hasNormal),
    _textureFilterMin(textureFilterMin),
    _textureFilterMag(textureFilterMag),
    _textureFilterMipMap(textureFilterMipMap)
    {
        // I normally don't allow definitions in headers, but sometimes I'm cool with it
    }
};

#endif /* defined(__noctisgames__NGTextureDesc__) */
