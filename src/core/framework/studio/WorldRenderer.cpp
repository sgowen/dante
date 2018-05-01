//
//  WorldRenderer.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/1/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/studio/WorldRenderer.h>

#include <framework/graphics/portable/RendererHelper.h>
#include <framework/studio/World.h>
#include <framework/entity/Entity.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/NGTexture.h>

WorldRenderer::WorldRenderer(RendererHelper* rendererHelper) : _rendererHelper(rendererHelper)
{
    // Empty
}

WorldRenderer::~WorldRenderer()
{
    // Empty
}

void WorldRenderer::config(WorldRendererConfig config)
{
    _config = config;
}
