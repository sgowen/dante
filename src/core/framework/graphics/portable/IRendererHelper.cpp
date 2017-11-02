//
//  IRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IRendererHelper.h"

#include "TextureWrapper.h"

IRendererHelper::IRendererHelper() : m_framebuffer(new TextureWrapper("framebuffer", NULL, false))
{
    // Empty
}

IRendererHelper::~IRendererHelper()
{
    // Empty
}
