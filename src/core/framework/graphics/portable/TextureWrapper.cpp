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

TextureWrapper::TextureWrapper(std::string inName, Renderer* renderer, bool in_repeatS) : name(inName), gpuTextureDataWrapper(nullptr), gpuTextureWrapper(nullptr), _renderer(renderer), repeatS(in_repeatS), isLoadingData(false)
{
    // Empty
}
