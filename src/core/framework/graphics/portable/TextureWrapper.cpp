//
//  TextureWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "TextureWrapper.h"

#include "GpuTextureDataWrapper.h"
#include "GpuTextureWrapper.h"

TextureWrapper::TextureWrapper(std::string inName, bool in_repeatS) : name(inName), gpuTextureDataWrapper(nullptr), gpuTextureWrapper(nullptr), repeatS(in_repeatS), isLoadingData(false)
{
    // Empty
}
