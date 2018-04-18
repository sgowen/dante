//
//  GameEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameEngine__
#define __noctisgames__GameEngine__

#include <framework/main/portable/EngineState.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteam.h>
#endif

#define GAME_ENGINE_CALLBACKS GameEngine::sHandleDynamicEntityCreatedOnClient, GameEngine::sHandleDynamicEntityDeletedOnClient

class Entity;
class GameRenderer;
class World;
class Timing;
class GameInputManager;

enum GameEngineState
{
    GameEngineState_Default = 1 << 0,
    GameEngineState_DisplayBox2D = 1 << 1,
    GameEngineState_Interpolation = 1 << 2,
    GameEngineState_Lighting = 1 << 3,
    GameEngineState_DisplayUI = 1 << 4
};

class GameEngine : public EngineState
{
    friend class GameInputManager;
    friend class GameRenderer;
    
    DECL_RTTI;
    
public:
    static void create();
    static GameEngine* getInstance();
    static void destroy();
    
    static uint64_t sGetPlayerAddressHash(uint8_t inPlayerIndex);
    static void sHandleDynamicEntityCreatedOnClient(Entity* entity);
    static void sHandleDynamicEntityDeletedOnClient(Entity* entity);
    
    virtual void enter(Engine* engine);
    virtual void update(Engine* engine);
    virtual void exit(Engine* engine);
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    virtual void onResume();
    virtual void onPause();
    virtual void render(double alpha);
    
    World* getWorld();
    
private:
    static GameEngine* s_instance;
    
    GameRenderer* _renderer;
    World* _world;
    Timing* _timing;
    GameInputManager* _input;
    uint32_t _state;
    uint32_t _map;
    
    GameEngine();
    virtual ~GameEngine();
};

#endif /* defined(__noctisgames__GameEngine__) */
