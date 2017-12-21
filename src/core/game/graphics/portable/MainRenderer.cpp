//
//  MainRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/portable/MainRenderer.h"

#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/Font.h"
#include "Box2D/Box2D.h"
#include "game/logic/World.h"

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
#include "game/logic/Robot.h"
#include "game/logic/Projectile.h"
#include "game/logic/SpacePirate.h"
#include "framework/util/StringUtil.h"
#include "framework/util/WeightedTimedMovingAverage.h"
#include "framework/util/NGSTDUtil.h"
#include "game/network/InstanceManager.h"
#include "game/logic/InputManager.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/MachineAddress.h"
#include "game/logic/MainEngineState.h"
#include "framework/math/MathUtil.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/network/Server.h"
#include "game/logic/Crate.h"
#include "game/logic/SpacePirateChunk.h"
#include "framework/util/FPSUtil.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/NGRectBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/util/FlagUtil.h"
#include "framework/util/FrameworkConstants.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <ctime> // rand

MainRenderer::MainRenderer() : Renderer(),
_characters(new NGTexture("texture_001.ngt", this, false, true)),
_misc(new NGTexture("texture_002.ngt", this, false, true)),
_bg1(new NGTexture("texture_003.ngt", this, true, true)),
_bg2(new NGTexture("texture_004.ngt", this, true, true)),
_font(new Font("texture_002.ngt", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024)),
_camBounds(new NGRect(0, 0, CAM_WIDTH, CAM_HEIGHT))
{
    // Empty
}

MainRenderer::~MainRenderer()
{
	releaseDeviceDependentResources();

	delete _characters;
	delete _misc;
    delete _bg1;
    delete _bg2;
    delete _font;
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
    
    const char* assetsCfgFileName = "assets.cfg";
    const char* finalAssetsCfgFileName;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s("data/textures/");
    s += std::string(assetsCfgFileName);
    finalAssetsCfgFileName = s.c_str();
#elif defined _WIN32
    std::string s("data\\textures\\");
    s += std::string(assetsCfgFileName);
    finalAssetsCfgFileName = s.c_str();
#else
    finalAssetsCfgFileName = assetsCfgFileName;
#endif
    
    ASSETS->initWithJsonFile(finalAssetsCfgFileName, true);
    
    loadTextureSync(_characters);
    loadTextureSync(_misc);
    loadTextureSync(_bg1);
    loadTextureSync(_bg2);
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();

	unloadTexture(_characters);
	unloadTexture(_misc);
    unloadTexture(_bg1);
    unloadTexture(_bg2);
}

void MainRenderer::render(int flags)
{
    beginFrame();
    
    _rendererHelper->clearFramebufferWithColor(0.0f, 0.0f, 0.0f, 1);
    
    if (ensureTexture(_characters)
        && ensureTexture(_misc)
        && ensureTexture(_bg1)
        && ensureTexture(_bg2))
    {
        updateCamera();
        
        renderBackground();
        
        if (InstanceManager::getClientWorld())
        {
            renderWorld();
        }
        
#ifdef NG_TEST_RENDERING_SUITE
        testRenderingSuite();
#endif
        
        renderUI(flags);
    }
    
    renderToScreen();
    
    endFrame();
}

void MainRenderer::renderBackground()
{
    for (int i = 0; i < 3; ++i)
    {
        _rendererHelper->updateMatrix(0, GAME_WIDTH, 0, CAM_HEIGHT);
        
        _spriteBatcher->beginBatch();
        {
            static TextureRegion tr = ASSETS->findTextureRegion("Background1");
            tr.initX(_camBounds->getLeft() * 128.0f / 3);
            tr.initY(clamp(384 - _camBounds->getBottom() * 32, 384, 0));
            _spriteBatcher->renderSprite(i * CAM_WIDTH + CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
        }
        _spriteBatcher->endBatch(_bg1, *_textureShaderProgram);
        
        _spriteBatcher->beginBatch();
        {
            static TextureRegion tr = ASSETS->findTextureRegion("Background2");
            tr.initX(_camBounds->getLeft() * 128.0f / 2);
            tr.initY(clamp(644 - _camBounds->getBottom() * 48, 644, 0));
            _spriteBatcher->renderSprite(i * CAM_WIDTH + CAM_WIDTH / 2, CAM_HEIGHT * 0.3875f / 2, CAM_WIDTH, CAM_HEIGHT * 0.3875f, 0, tr);
        }
        _spriteBatcher->endBatch(_bg2, *_textureShaderProgram);
    }
}

void MainRenderer::renderWorld()
{
    _rendererHelper->updateMatrix(_camBounds->getLeft(), _camBounds->getRight(), _camBounds->getBottom(), _camBounds->getTop());
    
    _spriteBatcher->beginBatch();
    for (int i = 0; i < 3; ++i)
    {
        static TextureRegion tr = ASSETS->findTextureRegion("Background3");
        _spriteBatcher->renderSprite(i * CAM_WIDTH + CAM_WIDTH / 2, CAM_HEIGHT * 0.2f / 2, CAM_WIDTH, CAM_HEIGHT * 0.2f, 0, tr);
    }
    _spriteBatcher->endBatch(_bg2, *_textureShaderProgram);
    
    renderEntities(InstanceManager::getClientWorld(), false);
    
    if (Server::getInstance() && Server::getInstance()->isDisplaying())
    {
        renderEntities(InstanceManager::getServerWorld(), true);
    }
    
    _spriteBatcher->beginBatch();
    std::vector<Entity*> entities = InstanceManager::getClientWorld()->getEntities();
    for (Entity* go : entities)
    {
        if (go->getNetworkType() == NW_TYPE_SpacePirate)
        {
            SpacePirate* entity = static_cast<SpacePirate*>(go);
            b2Vec2 origin = b2Vec2(entity->getPosition().x, entity->getPosition().y);
            origin += b2Vec2(0, entity->getHeight() / 2);
            std::string text = StringUtil::format("%i", entity->getHealth());
            renderText(text, origin, Color::DARK_RED, FONT_ALIGN_CENTERED);
        }
    }
    _spriteBatcher->endBatch(_misc, *_textureShaderProgram);
}

void MainRenderer::renderEntities(World* world, bool isServer)
{
    _spriteBatcher->beginBatch();
    std::vector<Entity*> entities = world->getEntities();
    for (Entity* go : entities)
    {
        Color c = Color(go->getColor().red, go->getColor().green, go->getColor().blue, go->getColor().alpha);
        if (isServer)
        {
            c.alpha = 0.5f;
        }
        
        if (go->getNetworkType() == NW_TYPE_Projectile)
        {
            Projectile* proj = static_cast<Projectile*>(go);
            if (proj->getState() != Projectile::ProjectileState::ProjectileState_Waiting)
            {
                bool isActive = proj->getState() == Projectile::ProjectileState::ProjectileState_Active;
                TextureRegion tr = isActive ? ASSETS->findTextureRegion("Projectile") : ASSETS->findTextureRegion("Explosion", proj->getStateTime());
                
                renderEntityWithColor(*proj, tr, c, proj->isFacingLeft());
            }
        }
        else if (go->getNetworkType() == NW_TYPE_SpacePirate)
        {
            SpacePirate* sp = static_cast<SpacePirate*>(go);
            TextureRegion tr = ASSETS->findTextureRegion("Space_Pirate_Walking", sp->getStateTime());
            
            renderEntityWithColor(*sp, tr, c, sp->isFacingLeft());
        }
        else if (go->getNetworkType() == NW_TYPE_Crate)
        {
            Crate* crate = static_cast<Crate*>(go);
            static Color activeColor = Color(1.0f, 0, 0, 1.0f);
            TextureRegion tr = ASSETS->findTextureRegion("Crate", go->getStateTime());
            
            activeColor.alpha = isServer ? 0.5f : 1.0f;
            
            renderEntityWithColor(*go, tr, crate->getBody()->IsAwake() ? activeColor : c);
        }
        else if (go->getNetworkType() == NW_TYPE_SpacePirateChunk)
        {
            SpacePirateChunk* spc = static_cast<SpacePirateChunk*>(go);
            int type = spc->getType();
            static TextureRegion tr0 = ASSETS->findTextureRegion("Space_Pirate_Chunk_Top_Left");
            static TextureRegion tr1 = ASSETS->findTextureRegion("Space_Pirate_Chunk_Top_Right");
            static TextureRegion tr2 = ASSETS->findTextureRegion("Space_Pirate_Chunk_Bottom_Left");
            static TextureRegion tr3 = ASSETS->findTextureRegion("Space_Pirate_Chunk_Bottom_Right");
            
            TextureRegion* tr = NULL;
            switch (type)
            {
                case Space_Pirate_Chunk_Top_Left:
                    tr = &tr0;
                    break;
                case Space_Pirate_Chunk_Top_Right:
                    tr = &tr1;
                    break;
                case Space_Pirate_Chunk_Bottom_Left:
                    tr = &tr2;
                    break;
                case Space_Pirate_Chunk_Bottom_Right:
                    tr = &tr3;
                    break;
                default:
                    break;
            }
            
            renderEntityWithColor(*go, *tr, c, spc->isFacingLeft());
        }
    }
    
    std::vector<Entity*> players = world->getPlayers();
    for (Entity* entity : players)
    {
        Robot* r = static_cast<Robot*>(entity);
        
        Color c = r->getColor();
        if (r->isSprinting())
        {
            c = Color(c.red, c.green, c.blue, c.alpha);
            c.alpha = (rand() % 50) * 0.01f + 0.25f;
        }
        
        bool isMoving = r->getVelocity().x < -0.5f || r->getVelocity().x > 0.5f;
        TextureRegion tr = ASSETS->findTextureRegion(!isMoving ? "Bot_Idle" : r->isSprinting() ? "Bot_Running_Fast" : "Bot_Running", r->getStateTime());
        
        if (isServer)
        {
            c.alpha /= 2.0f;
        }
        
        renderEntityWithColor(*r, tr, c, r->isFacingLeft());
    }
    
    _spriteBatcher->endBatch(_characters, *_textureShaderProgram);
}

void MainRenderer::renderUI(int flags)
{
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    _spriteBatcher->beginBatch();
    
    int state = flags;
    switch (state)
    {
        case MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF:
            renderMainMenuSteamOffText();
            break;
        case MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON:
            renderMainMenuSteamOnText();
            break;
        case MAIN_ENGINE_STATE_MAIN_MENU_STARTING_SERVER:
            renderStartingServerText();
            break;
        case MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME:
            renderEnterUsernameText();
            break;
        case MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP:
            renderJoiningLocalServerByIPText();
            break;
        case MAIN_ENGINE_STEAM_JOINING_SERVER:
            renderJoiningServerText();
            
            if (NG_CLIENT->getState() == NCS_Welcomed)
            {
                renderServerJoinedText(flags);
            }
            break;
        default:
            break;
    }
    _spriteBatcher->endBatch(_misc, *_textureShaderProgram);
}

void MainRenderer::renderMainMenuSteamOffText()
{
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
        std::string text = std::string("'A' to activate Steam");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("'S' to start local server");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 3);
        std::string text = std::string("'J' to join server by IP");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 8);
        std::string text = std::string("'ESC' to exit game");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
}

void MainRenderer::renderMainMenuSteamOnText()
{
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
        std::string text = std::string("'D' to deactivate Steam");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("'S' to start steam server");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 3);
        std::string text = std::string("'L' to refresh list of LAN servers");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = std::string("'I' to refresh list of Internet servers");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
#ifdef NG_STEAM
    std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
    int index = 0;
    for (NGSteamGameServer gameServer : gameServers)
    {
        b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 5.5f - (index * 0.5f));
        std::string text = StringUtil::format("'%i' %s", (++index), gameServer.getDisplayString());
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 8);
        std::string text = std::string("'ESC' to exit game");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
#endif
}

void MainRenderer::renderStartingServerText()
{
    static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
    std::string text = std::string("Server starting, 'ESC' to exit");
    renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
}

void MainRenderer::renderEnterUsernameText()
{
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("Enter Username to join, 'ESC' to exit");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = InputManager::getInstance()->getLiveInputRef();
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
}

void MainRenderer::renderJoiningLocalServerByIPText()
{
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("Enter Server Address to join, 'ESC' to exit");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = InputManager::getInstance()->getLiveInputRef();
        renderText(text, origin, Color::BLACK, FONT_ALIGN_CENTERED);
    }
}

void MainRenderer::renderJoiningServerText()
{
    {
        static b2Vec2 origin = b2Vec2(0.5f, CAM_HEIGHT - 3.5f);
        std::string text = StringUtil::format("%s, 'ESC' to exit", NG_CLIENT->getState() == NCS_Welcomed ? "Server joined" : "Joining Server...");
        renderText(text, origin, Color::BLACK, FONT_ALIGN_LEFT);
    }
}

void MainRenderer::renderServerJoinedText(int flags)
{
    static int row = 1;
    static float padding = 0.5f;
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
        int fps = FPSUtil::getInstance()->getFPS();
        std::string text = StringUtil::format("FPS %d", fps);
        renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        std::string text = StringUtil::format("RTT %d ms", static_cast<int>(rttMS));
        renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        
        std::string text = StringUtil::format(" In %d Bps", bpsInInt);
        renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
    }
    
    {
        static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        
        std::string text = StringUtil::format("Out %d Bps", bpsOutInt);
        renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
    }
    
    // Controls
    ++row;
    
    {
        {
            static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
            
            std::string text = StringUtil::format("'S'       Sound %s", NG_AUDIO_ENGINE->isSoundDisabled() ? "OFF" : " ON");
            renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
        }
        
        {
            static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
            
            std::string text = StringUtil::format("'M'       Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? "OFF" : " ON");
            renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
        }
    }
    
    if (Server::getInstance())
    {
        {
            static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
            
            std::string text = StringUtil::format("'E'     Enemies %s", Server::getInstance()->isSpawningEnemies() ? " ON" : "OFF");
            renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
        }
        
        {
            static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
            
            std::string text = StringUtil::format("'O'     Objects %s", Server::getInstance()->isSpawningObjects() ? " ON" : "OFF");
            renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
        }
        
        {
            static b2Vec2 origin = b2Vec2(CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding));
            
            std::string text = StringUtil::format("'I'       DEBUG %s", Server::getInstance()->isDisplaying() ? " ON" : "OFF");
            renderText(text, origin, Color::BLACK, FONT_ALIGN_RIGHT);
        }
    }
    
    if (InstanceManager::getClientWorld())
    {
        bool activePlayerIds[4] = {false};
        
        int enemyCount = 0;
        int objectCount = 0;
        std::vector<Entity*> entities = InstanceManager::getClientWorld()->getEntities();
        for (Entity* go : entities)
        {
            if (go->getNetworkType() == NW_TYPE_SpacePirate)
            {
                ++enemyCount;
            }
            else if (go->getNetworkType() == NW_TYPE_Crate)
            {
                ++objectCount;
            }
        }
        
        std::vector<Entity*> players = InstanceManager::getClientWorld()->getPlayers();
        for (Entity* entity : players)
        {
			Robot* robot = static_cast<Robot*>(entity);
            
			int playerId = robot->getPlayerId();
			if (playerId >= 1 && playerId <= 4)
			{
				b2Vec2 origin = b2Vec2(0.5f, CAM_HEIGHT - (playerId * 0.5f));
				std::string text = StringUtil::format("%i|%s - %i HP, %i Kills", playerId, robot->getPlayerName().c_str(), robot->getHealth(), robot->getNumKills());
				renderText(text, origin, Color::BLACK);

				activePlayerIds[playerId - 1] = true;
			}
        }
        
        for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            if (!activePlayerIds[i])
            {
                b2Vec2 origin = b2Vec2(0.5f, CAM_HEIGHT - ((i + 1) * 0.5f));
                std::string text = StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...");
                renderText(text, origin, Color::BLACK);
            }
        }
        
        {
            b2Vec2 origin = b2Vec2(0.5f, CAM_HEIGHT - 4.0f);
            std::string text = StringUtil::format("Enemies: %i", enemyCount);
            renderText(text, origin, Color::BLACK, FONT_ALIGN_LEFT);
        }
        
        {
            b2Vec2 origin = b2Vec2(0.5f, CAM_HEIGHT - 4.5f);
            std::string text = StringUtil::format("Objects: %i", objectCount);
            renderText(text, origin, Color::BLACK, FONT_ALIGN_LEFT);
        }
    }
}

void MainRenderer::renderText(const std::string& inStr, const b2Vec2& origin, const Color& inColor, int justification)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    _font->renderText(*_spriteBatcher, text, origin.x, origin.y, fgWidth, fgHeight, fontColor, justification);
}

void MainRenderer::updateCamera()
{
    _camBounds->getLowerLeft().set(0, 0);
    _camBounds->setWidth(CAM_WIDTH);
    _camBounds->setHeight(CAM_HEIGHT);
    
    if (NG_CLIENT && NG_CLIENT->getState() == NCS_Welcomed)
    {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        
        bool needsInit = true;
        
        for (Entity* entity : InstanceManager::getClientWorld()->getPlayers())
        {
            Robot* player = static_cast<Robot*>(entity);
            
            if (NG_CLIENT->isPlayerIdLocal(player->getPlayerId()))
            {
                // Adjust camera based on the player position
                
                float pX = player->getPosition().x;
                float pY = player->getPosition().y;
                
                if (needsInit)
                {
                    left = pX;
                    right = pX;
                    bottom = pY;
                    top = pY;
                    
                    needsInit = false;
                }
                else
                {
                    left = MIN(pX, left);
                    right = MAX(pX, right);
                    
                    bottom = MIN(pY, bottom);
                    top = MAX(pY, top);
                }
            }
        }
        
        if (!needsInit)
        {
            left -= CAM_WIDTH * 0.33f;
            right += CAM_WIDTH * 0.33f;
            bottom -= CAM_HEIGHT * 0.33f;
            top += CAM_HEIGHT * 0.33f;
            
            float pX = (left + right) / 2.0f;
            float pY = (bottom + top) / 2.0f;
            float w = right - left;
            float h = top - bottom;
            
            if (w < CAM_WIDTH)
            {
                w = CAM_WIDTH;
            }
            
            if (h < CAM_HEIGHT)
            {
                h = CAM_HEIGHT;
            }
            
            if (w > CAM_WIDTH)
            {
                h = w * (CAM_HEIGHT / CAM_WIDTH);
            }
            else if (h > CAM_HEIGHT)
            {
                w = h * (CAM_WIDTH / CAM_HEIGHT);
            }
            
            float x = pX - w * 0.5f;
            
            float y = pY - h * 0.5f;
            y = clamp(y, GAME_HEIGHT, 0);
            
            _camBounds->getLowerLeft().set(x, y);
            _camBounds->setWidth(w);
            _camBounds->setHeight(h);
        }
    }
}
