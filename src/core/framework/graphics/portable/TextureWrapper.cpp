//
//  TextureWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/TextureWrapper.h"

#include "framework/graphics/portable/GpuTextureDataWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/portable/Renderer.h"

TextureWrapper::TextureWrapper(std::string inName, Renderer* renderer, bool repeatS, bool isEncrypted) : name(inName), gpuTextureDataWrapper(NULL), gpuTextureWrapper(NULL), _renderer(renderer), _repeatS(repeatS), _isEncrypted(isEncrypted), _isLoadingData(false)
{
    // Empty
}
