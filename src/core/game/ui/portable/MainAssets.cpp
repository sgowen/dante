//
//  MainAssets.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainAssets.h"

#include "Assets.h"

#include "TextureRegion.h"

MainAssets* MainAssets::getInstance()
{
    static MainAssets instance = MainAssets();
    return &instance;
}

bool MainAssets::isUsingCompressedTextureSet()
{
    return m_isUsingCompressedTextureSet;
}

void MainAssets::setUsingCompressedTextureSet(bool isUsingCompressedTextureSet)
{
    m_isUsingCompressedTextureSet = isUsingCompressedTextureSet;
}

bool MainAssets::isUsingDesktopTextureSet()
{
    return m_isUsingDesktopTextureSet;
}

void MainAssets::setUsingDesktopTextureSet(bool isUsingDesktopTextureSet)
{
    m_isUsingDesktopTextureSet = isUsingDesktopTextureSet;
}

bool MainAssets::isUsingGamePadTextureSet()
{
    return m_isUsingGamePadTextureSet;
}

void MainAssets::setUsingGamePadTextureSet(bool isUsingGamePadTextureSet)
{
    m_isUsingGamePadTextureSet = isUsingGamePadTextureSet;
}

MainAssets::MainAssets() : m_isUsingCompressedTextureSet(false), m_isUsingDesktopTextureSet(false), m_isUsingGamePadTextureSet(false)
{
    // Empty
}
