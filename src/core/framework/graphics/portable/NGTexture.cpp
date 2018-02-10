//
//  NGTexture.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGTexture.h>

#include <framework/graphics/portable/TextureDataWrapper.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/TextureManager.h>
#include <framework/graphics/portable/NGTextureDesc.h>

#include <framework/file/portable/FileUtil.h>

NGTexture::NGTexture(std::string inName, TextureManager* textureManager, NGTextureDesc* desc) : name(inName), textureDataWrapper(NULL), textureWrapper(NULL), _textureManager(textureManager), _desc(desc), _isFramebuffer(false), _isLoadingData(false)
{
    if (name == "framebuffer")
    {
        _isFramebuffer = true;
    }
    
    filePath = FileUtil::filePathForTexture(name.c_str());
}
