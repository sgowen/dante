//
//  StudioRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/portable/StudioRenderer.h"

#include "framework/graphics/portable/TextureManager.h"
#include "framework/graphics/portable/Font.h"
#include "game/logic/World.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/PolygonBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/NGShaderLoader.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/entity/Entity.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"

#include "framework/graphics/portable/Assets.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "game/logic/GameConstants.h"
#include "framework/math/NGRect.h"
#include "framework/math/Line.h"
#include "framework/graphics/portable/Color.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/util/macros.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include "framework/util/WeightedTimedMovingAverage.h"
#include "framework/util/NGSTDUtil.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/TitleInputManager.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/math/MathUtil.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/Server.h"
#include "game/logic/TitleEngine.h"
#include "framework/util/FPSUtil.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/PolygonBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/graphics/portable/NGShaderLoaderFactory.h"
#include "framework/graphics/portable/RendererHelperFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/math/Circle.h"
#include "framework/math/Line.h"
#include <framework/graphics/portable/NGTextureShader.h>
#include <framework/graphics/portable/NGGeometryShader.h>
#include <framework/graphics/portable/NGFramebufferToScreenShader.h>
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/Assets.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>

StudioRenderer::StudioRenderer() : Renderer(),
_textureManager(new TextureManager("title_assets.cfg")),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_spriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createNGShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")),
_colorNGShader(new NGGeometryShader(*_rendererHelper, "shader_001_vert.ngs", "shader_001_frag.ngs")),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")),
_font(new Font("texture_001.ngt", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024)),
_framebufferIndex(0)
{
    // Empty
}

StudioRenderer::~StudioRenderer()
{
    releaseDeviceDependentResources();
    
    delete _textureManager;
    delete _rendererHelper;
    delete _spriteBatcher;
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _shaderProgramLoader;
    delete _font;
    delete _textureNGShader;
    delete _colorNGShader;
    delete _framebufferToScreenNGShader;
}

void StudioRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void StudioRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
}

void StudioRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();
    
    _textureNGShader->unload(*_shaderProgramLoader);
    _colorNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void StudioRenderer::render(int flags)
{
    beginFrame();
    
    if (_textureManager->ensureTextures())
    {
        _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
        
        _spriteBatcher->beginBatch();
        
        renderText("Awwww yeah, prepare for the Studio!!!", CAM_WIDTH / 2, CAM_HEIGHT - 2, Color::WHITE, FONT_ALIGN_CENTERED);
        
        renderText("'ESC' to exit",                         CAM_WIDTH / 2, CAM_HEIGHT - 9, Color::WHITE, FONT_ALIGN_CENTERED);
        
        _spriteBatcher->endBatch(_textureManager->getTextureWithName("texture_000.ngt"), *_textureNGShader);
    }
    
    endFrame();
}

void StudioRenderer::beginFrame()
{
    _textureManager->handleAsyncTextureLoads();
    
    setFramebuffer(0);
}

void StudioRenderer::setFramebuffer(int framebufferIndex)
{
    assert(framebufferIndex >= 0);
    
    _framebufferIndex = framebufferIndex;
    
    _rendererHelper->bindToOffscreenFramebuffer(_framebufferIndex);
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
}

void StudioRenderer::renderText(const char* inStr, float x, float y, const Color& inColor, int justification)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*_spriteBatcher, text, x, y, fgWidth, fgHeight, fontColor, justification);
}

void StudioRenderer::endFrame()
{
    assert(_framebufferIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    static std::vector<SCREEN_VERTEX> screenVertices;
    screenVertices.clear();
    screenVertices.push_back(SCREEN_VERTEX(-1, -1));
    screenVertices.push_back(SCREEN_VERTEX(-1, 1));
    screenVertices.push_back(SCREEN_VERTEX(1, 1));
    screenVertices.push_back(SCREEN_VERTEX(1, -1));
    
    _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(_framebufferIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();
}
