//
//  RendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RendererHelper.h"

#include "TextureWrapper.h"

RendererHelper::RendererHelper() : _framebuffer(new TextureWrapper("framebuffer", NULL, false))
{
    // Empty
}

RendererHelper::~RendererHelper()
{
    // Empty
}
