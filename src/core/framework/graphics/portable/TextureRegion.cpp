//
//  TextureRegion.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/TextureRegion.h"

TextureRegion::TextureRegion(std::string textureName, int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight)
{
    init(x, y, regionWidth, regionHeight, textureWidth, textureHeight);
}

void TextureRegion::init(int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight)
{
    _x = (double) x;
    _y = (double) y;
    _regionWidth = (double) regionWidth;
    _regionHeight = (double) regionHeight;
    _textureWidth = (double) textureWidth;
    _textureHeight = (double) textureHeight;
    
    _u1 = _x / _textureWidth;
    _v1 = _y / _textureHeight;
    _u2 = _u1 + _regionWidth / _textureWidth;
    _v2 = _v1 + _regionHeight / _textureHeight;
}

void TextureRegion::initX(int x)
{
    _x = (double) x;
    
    _u1 = _x / _textureWidth;
    _u2 = _u1 + _regionWidth / _textureWidth;
}

void TextureRegion::initY(int y)
{
    _y = (double) y;
    
    _v1 = _y / _textureHeight;
    _v2 = _v1 + _regionHeight / _textureHeight;
}

std::string& TextureRegion::getTextureName()
{
    return _textureName;
}
