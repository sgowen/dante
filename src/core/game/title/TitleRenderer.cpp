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
#include <game/logic/World.h>
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
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/util/StringUtil.h>
#include <framework/util/WeightedTimedMovingAverage.h>
#include <framework/util/NGSTDUtil.h>
#include <game/title/TitleInputManager.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/util/MathUtil.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/logic/Server.h>
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
#include <game/logic/GameConfig.h>
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
_spriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper)),
_colorNGShader(new NGGeometryShader(*_rendererHelper)),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper)),
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

void TitleRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();

    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void TitleRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight);
}

void TitleRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();

    _textureNGShader->unload(*_shaderProgramLoader);
    _colorNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void TitleRenderer::render()
{
    _engineState = _engine->_state;
    
    bindFramebuffer(0);
    _rendererHelper->useNormalBlending();

    if (_textureManager->ensureTextures())
    {
        _rendererHelper->updateMatrix(0, GM_CFG->_camWidth, 0, GM_CFG->_camHeight);

        _spriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);

        switch (_engineState)
        {
            case TitleEngineState_SteamOff:
                renderMainMenuSteamOffText();
                break;
            case TitleEngineState_SteamOn:
                renderMainMenuSteamOnText();
                break;
            case TitleEngineState_ServerStarting:
                renderStartingServerText();
                break;
            case TitleEngineState_InputName:
                renderEnterUsernameText();
                break;
            case TitleEngineState_InputIp:
                renderJoiningLocalServerByIPText();
                break;
            default:
                break;
        }

        _spriteBatcher->endBatch(_textureNGShader, _textureManager->getTextureWithName("texture_000.ngt"));
    }
    
    //testRenderingSuite();

    endFrame();
}

void TitleRenderer::setEngine(TitleEngine* inValue)
{
    _engine = inValue;
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
    if (PlatformHelper::getPlatform() != NG_PLATFORM_ANDROID
        && PlatformHelper::getPlatform() != NG_PLATFORM_IOS)
    {
        renderText("[E] to enter Studio",   GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 2, FONT_ALIGN_LEFT);
        renderText("[A] to activate Steam", GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 4, FONT_ALIGN_LEFT);
    }
    
    renderText("[S] to start local server", GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 6, FONT_ALIGN_LEFT);
    renderText("[J] to join server by IP",  GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 8, FONT_ALIGN_LEFT);

    renderText("[ESC] to exit game", GM_CFG->_camWidth / 2, 4, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderMainMenuSteamOnText()
{
    renderText("[E] to enter Studio",                     GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 2, FONT_ALIGN_LEFT);
    renderText("[D] to deactivate Steam",                 GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 4, FONT_ALIGN_LEFT);
    renderText("[S] to start steam server",               GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 6, FONT_ALIGN_LEFT);
    renderText("[L] to refresh list of LAN servers",      GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 8, FONT_ALIGN_LEFT);
    renderText("[I] to refresh list of Internet servers", GM_CFG->_camWidth / 4, GM_CFG->_camHeight - 10, FONT_ALIGN_LEFT);
    
#ifdef NG_STEAM
    std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
    int index = 0;
    for (NGSteamGameServer gameServer : gameServers)
    {
        int serverNumber = index + 1;
        renderText(StringUtil::format("[%i] %s", serverNumber, gameServer.getDisplayString()).c_str(), GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 13.0f - (index * 0.5f), FONT_ALIGN_CENTER);

        ++index;
    }
#endif

    renderText("[ESC] to exit game", GM_CFG->_camWidth / 2, 4, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderStartingServerText()
{
    renderText("Server starting, [ESC] to exit", GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 4, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderEnterUsernameText()
{
    renderText("Enter Username to join, [ESC] to exit", GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 4, FONT_ALIGN_CENTER);

    renderText(TitleInputManager::getInstance()->getLiveInputRef().c_str(), GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 8, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderJoiningLocalServerByIPText()
{
    renderText("Enter Server Address to join, [ESC] to exit", GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 4, FONT_ALIGN_CENTER);

    renderText(TitleInputManager::getInstance()->getLiveInputRef().c_str(), GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 8, FONT_ALIGN_CENTER);
}

void TitleRenderer::renderText(const char* inStr, float x, float y, int justification)
{
    float fgWidth = GM_CFG->_camWidth / 64;
    float fgHeight = fgWidth * 1.171875f;

    std::string text(inStr);

    _font->renderText(*_spriteBatcher, text, x, y, fgWidth, fgHeight, justification);
}

void TitleRenderer::testRenderingSuite()
{
    _rendererHelper->updateMatrix(0, 16, 0, 9);
    
    _circleBatcher->beginBatch();
    static Circle c1(10, 4, 2);
    _circleBatcher->renderCircle(c1);
    static Circle c2(7, 7, 2);
    _circleBatcher->renderPartialCircle(c2, 135);
    _circleBatcher->endBatch(_colorNGShader, Color::RED);
    
    static NGRect r1(1, 1, 2, 1);
    _boundsPolygonBatcher->beginBatch();
    _boundsPolygonBatcher->renderRect(r1);
    _boundsPolygonBatcher->endBatch(_colorNGShader, Color::RED);
    
    static NGRect r2(4, 1, 2, 1);
    _fillPolygonBatcher->beginBatch();
    _fillPolygonBatcher->renderRect(r2);
    _fillPolygonBatcher->endBatch(_colorNGShader, Color::RED);
    
    static Line line(3, 3, 5, 5);
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(line);
    _lineBatcher->endBatch(_colorNGShader, Color::RED);
}

void TitleRenderer::endFrame()
{
    assert(_fbIndex >= 0);

    _rendererHelper->bindScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    _framebufferToScreenNGShader->bind(_rendererHelper->getOffscreenFramebuffer(_fbIndex)->texture);
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();

    _rendererHelper->disableBlending();
}
