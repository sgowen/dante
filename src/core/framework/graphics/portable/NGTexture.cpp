//
//  NGTexture.cpp
//  noctisgames
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

NGTexture::NGTexture(std::string name, NGTextureDesc* desc, TextureManager* textureManager) :
_name(name),
_filePath(FileUtil::filePathForTexture(_name.c_str())),
_textureDataWrapper(NULL),
_textureWrapper(NULL),
_textureManager(textureManager),
_desc(desc),
_isLoadingData(false)
{
    // Empty
}
