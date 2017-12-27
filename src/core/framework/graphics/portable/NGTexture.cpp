//
//  NGTexture.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGTexture.h"

#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureManager.h"

NGTexture::NGTexture(std::string inName, TextureManager* textureManager, bool repeatS, bool isEncrypted) : name(inName), textureDataWrapper(NULL), textureWrapper(NULL), _textureManager(textureManager), _repeatS(repeatS), _isEncrypted(isEncrypted), _isLoadingData(false)
{
    // Empty
}
