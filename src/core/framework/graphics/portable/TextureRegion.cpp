//
//  TextureRegion.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/TextureRegion.h>

TextureRegion::TextureRegion(std::string textureName, int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight, int layer) :
_x(x),
_y(y),
_regionWidth(regionWidth),
_regionHeight(regionHeight),
_textureWidth(textureWidth),
_textureHeight(textureHeight),
_layer(layer),
_textureName(textureName),
_normalMapName("n_" + _textureName)
{
    _u1 = _x / (double) _textureWidth;
    _v1 = _y / (double) _textureHeight;
    _u2 = _u1 + _regionWidth / (double) _textureWidth;
    _v2 = _v1 + _regionHeight / (double) _textureHeight;
}

std::string& TextureRegion::getTextureName()
{
    return _textureName;
}

std::string& TextureRegion::getNormalMapName()
{
    return _normalMapName;
}
