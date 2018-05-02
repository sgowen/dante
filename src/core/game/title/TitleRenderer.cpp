//
//  TitleRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/title/TitleRenderer.h>

#include <framework/graphics/portable/TextureManager.h>
#include <framework/graphics/portable/Font.h>
#include <framework/studio/World.h>
#include <framework/graphics/portable/Box2DDebugRenderer.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShader.h>
#include <framework/entity/Entity.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/math/NGRect.h>
#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/Box2DDebugRenderer.h>

#include <framework/graphics/portable/Color.h>
#include <framework/file/portable/Assets.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/util/Constants.h>
#include <framework/math/NGRect.h>
#include <framework/math/Line.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/util/macros.h>
#include <framework/util/StringUtil.h>
#include <framework/util/WeightedTimedMovingAverage.h>
#include <framework/util/NGSTDUtil.h>
#include <game/title/TitleInputManager.h>
#include <framework/util/MathUtil.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/game/Server.h>
#include <game/title/TitleEngine.h>
#include <framework/util/FPSUtil.h>
#include <framework/math/Circle.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/util/Constants.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/TextureDataWrapper.h>
#include <framework/graphics/portable/TextureLoaderFactory.h>
#include <framework/graphics/portable/ShaderProgramLoaderFactory.h>
#include <framework/graphics/portable/RendererHelperFactory.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/math/Circle.h>
#include <framework/math/Line.h>
#include <framework/graphics/portable/NGTextureShader.h>
#include <framework/graphics/portable/NGGeometryShader.h>
#include <framework/graphics/portable/NGFramebufferToScreenShader.h>
#include <framework/graphics/portable/NGTextureDesc.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/PlatformHelper.h>
#include <game/config/GameConfig.h>
#include <framework/util/Config.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamGameServer.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>

TitleRenderer::TitleRenderer() : Renderer(),
_textureManager(new TextureManager()),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderLoader()),
_spriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_textureShader(new NGTextureShader(*_rendererHelper)),
_colorShader(new NGGeometryShader(*_rendererHelper)),
_framebufferToScreenShader(new NGFramebufferToScreenShader(*_rendererHelper)),
_font(new Font(0, 0, 16, 64, 75, 1024, 1024)),
_fbIndex(0),
_engine(NULL),
_engineState(0)
{
    // Empty
}

TitleRenderer::~TitleRenderer()
{
    delete _textureManager;
    delete _rendererHelper;
    delete _shaderProgramLoader;
    delete _spriteBatcher;
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _font;
    delete _textureShader;
    delete _colorShader;
    delete _framebufferToScreenShader;
}

void TitleRenderer::createDeviceDependentResources()
{
    _engine = TitleEngine::getInstance();
    
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();

    _textureShader->load(*_shaderProgramLoader);
    _colorShader->load(*_shaderProgramLoader);
    _framebufferToScreenShader->load(*_shaderProgramLoader);
}

void TitleRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight);
}

void TitleRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();

    _textureShader->unload(*_shaderProgramLoader);
    _colorShader->unload(*_shaderProgramLoader);
    _framebufferToScreenShader->unload(*_shaderProgramLoader);
}

void TitleRenderer::render()
{
    _engineState = _engine->_state;
    
    bindFramebuffer(0);
    _rendererHelper->useNormalBlending();

    if (_textureManager->ensureTextures())
    {
        _rendererHelper->updateMatrix(0, FW_CFG->_camWidth, 0, FW_CFG->_camHeight);

        _spriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);

        switch (_engineState)
        {
            case TES_SteamOff:
                renderMainMenuSteamOffText();
                break;
            case TES_SteamOn:
                renderMainMenuSteamOnText();
                break;
            case TES_InputName:
                renderEnterUsernameText();
                break;
            case TES_InputIp:
                renderJoiningLocalServerByIPText();
                break;
            default:
                break;
        }

        _spriteBatcher->endBatch(_textureShader, _textureManager->getTextureWithName("texture_000.ngt"));
    }

    endFrame();
}

void TitleRenderer::bindFramebuffer(int fbIndex, float r, float g, float b, float a)
{
    assert(fbIndex >= 0);

    _fbIndex = fbIndex;

    FramebufferWrapper* fb = _rendererHelper->getOffscreenFramebuffer(_fbIndex);
    _rendererHelper->bindFramebuffer(fb);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void TitleRenderer::renderMainMenuSteamOffText()
{
    if (!PlatformHelper::isMobile())
    {
        renderText("[E] to enter Studio",   FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 2, FONT_ALIGN_LEFT);
        renderText("[A] to activate Steam", FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 4, FONT_ALIGN_LEFT);
    }
    
    renderText("[S] to start local server", FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 6, FONT_ALIGN_LEFT);
    renderText("[J] to join server by IP",  FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 8, FONT_ALIGN_LEFT);

    renderText("[ESC] to exit game", FW_CFG->_camWidth / 2, 4, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderMainMenuSteamOnText()
{
    renderText("[E] to enter Studio",                     FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 2, FONT_ALIGN_LEFT);
    renderText("[D] to deactivate Steam",                 FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 4, FONT_ALIGN_LEFT);
    renderText("[S] to start steam server",               FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 6, FONT_ALIGN_LEFT);
    renderText("[L] to refresh list of LAN servers",      FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 8, FONT_ALIGN_LEFT);
    renderText("[I] to refresh list of Internet servers", FW_CFG->_camWidth / 4, FW_CFG->_camHeight - 10, FONT_ALIGN_LEFT);
    
#ifdef NG_STEAM
    std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
    int index = 0;
    for (NGSteamGameServer gameServer : gameServers)
    {
        int serverNumber = index + 1;
        renderText(StringUtil::format("[%i] %s", serverNumber, gameServer.getDisplayString()).c_str(), FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 13.0f - (index * 0.5f), FONT_ALIGN_CENTER);

        ++index;
    }
#endif

    renderText("[ESC] to exit game", FW_CFG->_camWidth / 2, 4, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderEnterUsernameText()
{
    renderText("Enter Username to join, [ESC] to exit", FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4, FONT_ALIGN_CENTER);

    renderText(TitleInputManager::getInstance()->getLiveInputRef().c_str(), FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 8, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderJoiningLocalServerByIPText()
{
    renderText("Enter Server Address to join, [ESC] to exit", FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4, FONT_ALIGN_CENTER);

    renderText(TitleInputManager::getInstance()->getLiveInputRef().c_str(), FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 8, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderText(const char* inStr, float x, float y, int justification)
{
    float fgWidth = FW_CFG->_camWidth / 64;
    float fgHeight = fgWidth * 1.171875f;

    std::string text(inStr);

    _font->renderText(*_spriteBatcher, text, x, y, fgWidth, fgHeight, justification);
}

void TitleRenderer::endFrame()
{
    assert(_fbIndex >= 0);

    _rendererHelper->bindScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    _framebufferToScreenShader->bind(_rendererHelper->getOffscreenFramebuffer(_fbIndex)->texture);
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenShader->unbind();

    _rendererHelper->disableBlending();
}
