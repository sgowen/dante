//
//  PlayerController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/entity/PlayerController.h>

#include <framework/entity/Entity.h>
#include <Box2D/Box2D.h>

#include <game/game/GameInputState.h>

#include <framework/studio/World.h>
#include <framework/util/macros.h>
#include <framework/util/Constants.h>
#include <framework/util/Timing.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <game/game/GameInputManager.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/game/Util.h>
#include <game/game/Server.h>
#include <framework/util/Config.h>
#include <game/config/GameConfig.h>
#include <game/game/GameEngine.h>

IMPL_RTTI(PlayerController, EntityController);

IMPL_EntityController_create(PlayerController);

PlayerController::PlayerController(Entity* e) : EntityController(e),
_playerInfo(),
_playerInfoNetworkCache(_playerInfo),
_stats(),
_statsNetworkCache(_stats),
_attackSensorFixture(NULL)
{
    // Empty
}

PlayerController::~PlayerController()
{
    // Empty
}

void PlayerController::update()
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    if ((state == State_FirstPunch && stateTime == 1) ||
        (state == State_SecondPunch && stateTime == 1) ||
        (state == State_ThirdPunch && stateTime == 1))
    {
        if (_stats.target > 0)
        {
            EntityManager* entityManager = _entity->getNetworkController()->isServer() ? NG_SERVER->getEntityManager() : NG_CLIENT->getEntityManager();
            Entity* e = entityManager->getEntityByID(_stats.target);
            if (e)
            {
                uint32_t damage = state == State_ThirdPunch ? 2 : 1;
                e->getController()->receiveMessage(ENTITY_MESSAGE_DAMAGE, &damage);
            }
        }
    }
    
    if (_entity->getNetworkController()->isServer())
    {
        if (_stats.health == 0)
        {
            _entity->requestDeletion();
        }
    }
}

void PlayerController::receiveMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case ENTITY_MESSAGE_DAMAGE:
        {
            uint8_t& health = _stats.health;
            
            uint32_t* damageP = static_cast<uint32_t*>(data);
            uint32_t& damage = *damageP;
            damage = clamp(damage, 0, health);
            health -= damage;
        }
            break;
        default:
            break;
    }
}

void PlayerController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    assert(fixtures.size() >= 4);
    _attackSensorFixture = fixtures[3];
}

bool PlayerController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Don't collide with other players
    return inEntity->getEntityDef().type != _entity->getEntityDef().type;
}

void PlayerController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor())
    {
        _stats.target = inEntity->getID();
    }
}

void PlayerController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor())
    {
        _stats.target = 0;
    }
}

void PlayerController::processInput(InputState* inInputState)
{
    GameInputState* is = static_cast<GameInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    GameInputState::PlayerInputState* playerInputState = is->getPlayerInputStateForID(playerId);
    if (playerInputState == NULL)
    {
        return;
    }
    
    uint8_t fromState = _entity->getState().state;
    uint8_t& state = _entity->getState().state;
    uint8_t inputState = playerInputState->getInputState();
    switch (state)
    {
        case State_Idle:
            processInputForIdleState(inputState);
            break;
        case State_FirstPunch:
            processInputForFirstPunchState(inputState);
            break;
        case State_SecondPunch:
            processInputForSecondPunchState(inputState);
            break;
        case State_ThirdPunch:
            processInputForThirdPunchState(inputState);
            break;
        case State_Running:
            processInputForRunningState(inputState);
            break;
        case State_Jumping:
            processInputForJumpingState(inputState);
            break;
        case State_JumpCompleted:
            processInputForFirstJumpCompletedState(inputState);
            break;
        default:
            break;
    }
    
    if (GameEngine::getInstance()->isLive())
    {
        Util::handleSound(_entity, fromState, state);
    }
}

void PlayerController::setAddressHash(uint64_t inValue)
{
    _playerInfo.addressHash = inValue;
}

uint64_t PlayerController::getAddressHash() const
{
    return _playerInfo.addressHash;
}

void PlayerController::setPlayerId(uint8_t inValue)
{
    _playerInfo.playerId = inValue;
}

uint8_t PlayerController::getPlayerId() const
{
    return _playerInfo.playerId;
}

void PlayerController::setPlayerName(std::string inValue)
{
    _playerInfo.playerName = inValue;
}

std::string& PlayerController::getPlayerName()
{
    return _playerInfo.playerName;
}

void PlayerController::processInputForIdleState(uint8_t inputState)
{
    handleMovementInput(inputState);
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleMainActionInput(inputState);
    }
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForFirstPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getState().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForSecondPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getState().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForThirdPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getState().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForRunningState(uint8_t inputState)
{
    handleMovementInput(inputState);
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleMainActionInput(inputState);
    }
    
    if (_entity->getState().state == State_Idle ||
        _entity->getState().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForJumpingState(uint8_t inputState)
{
    handleJumpInput(inputState);
}

void PlayerController::processInputForFirstJumpCompletedState(uint8_t inputState)
{
    handleJumpCompletedInput(inputState);
}

void PlayerController::handleMovementInput(uint8_t inputState)
{
    static const int LEFT = 0;
    static const int STOP = 1;
    static const int RIGHT = 2;
    
    uint8_t& state = _entity->getState().state;
    
    state = State_Idle;
    int moveState = STOP;
    if (_entity->isGrounded())
    {
        if (inputState & GameInputStateFlag_MovingRight)
        {
            state = State_Running;
            moveState = RIGHT;
        }
        else if (inputState & GameInputStateFlag_MovingLeft)
        {
            state = State_Running;
            moveState = LEFT;
        }
    }
    else
    {
        state = State_JumpCompleted;
    }
    
    const b2Vec2& vel = _entity->getVelocity();
    float desiredVel = 0;
    switch (moveState)
    {
        case LEFT:
            desiredVel = b2Max(vel.x - 1, -GM_CFG->_maxRobotVelocityX);
            break;
        case STOP:
            desiredVel = vel.x * 0.99f;
            break;
        case RIGHT:
            desiredVel = b2Min(vel.x + 1, GM_CFG->_maxRobotVelocityX);
            break;
    }
    
    float velChange = desiredVel - vel.x;
    float impulse = _entity->getBody()->GetMass() * velChange;
    
    _entity->getPose().isFacingLeft = moveState == LEFT ? true : moveState == RIGHT ? false : _entity->getPose().isFacingLeft;
    _entity->updateBodyFromPose();
    
    if (!isCloseEnough(impulse, 0))
    {
        _entity->getBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), _entity->getBody()->GetWorldCenter(), true);
    }
}

void PlayerController::handleMainActionInput(uint8_t inputState)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    uint8_t& stateFlags = _entity->getState().stateFlags;
    
    if (inputState & GameInputStateFlag_MainAction && _entity->isGrounded())
    {
        if (stateFlags == 0 && state != State_FirstPunch && state != State_SecondPunch && state != State_ThirdPunch)
        {
            state = State_FirstPunch;
            stateTime = 0;
            stateFlags = 0;
        }
        else if (stateFlags == MainActionFlag_ReadyForSecondPunch && stateTime < 30 && state != State_SecondPunch)
        {
            stateFlags = MainActionFlag_ToThrowSecondPunch;
        }
        else if (stateFlags == MainActionFlag_ReadyForThirdPunch && stateTime < 30 && state != State_ThirdPunch)
        {
            stateFlags = MainActionFlag_ToThrowThirdPunch;
        }
        
        if (stateFlags == MainActionFlag_ToThrowSecondPunch && stateTime > 18)
        {
            state = State_SecondPunch;
            stateTime = 0;
            stateFlags = 0;
        }
        else if (stateFlags == MainActionFlag_ToThrowThirdPunch && stateTime > 18)
        {
            state = State_ThirdPunch;
            stateTime = 0;
            stateFlags = 0;
        }
    }
    else
    {
        if (_entity->isGrounded())
        {
            if (state == State_FirstPunch)
            {
                if (stateFlags == 0)
                {
                    stateFlags = MainActionFlag_ReadyForSecondPunch;
                }
            }
            else if (state == State_SecondPunch)
            {
                if (stateFlags == 0)
                {
                    stateFlags = MainActionFlag_ReadyForThirdPunch;
                }
            }
            else if (state == State_ThirdPunch)
            {
                stateFlags = 0;
            }
        }
        else if (state == State_FirstPunch || state == State_SecondPunch || state == State_ThirdPunch)
        {
            state = State_Idle;
            stateFlags = 0;
        }
        
        if (stateFlags == MainActionFlag_ToThrowSecondPunch && stateTime > 18)
        {
            state = State_SecondPunch;
            stateTime = 0;
            stateFlags = 0;
        }
        else if (stateFlags == MainActionFlag_ToThrowThirdPunch && stateTime > 18)
        {
            state = State_ThirdPunch;
            stateTime = 0;
            stateFlags = 0;
        }
        
        if ((state == State_FirstPunch && stateTime > 30) ||
            (state == State_SecondPunch && stateTime > 30) ||
            (state == State_ThirdPunch && stateTime > 30))
        {
            state = State_Idle;
            stateFlags = 0;
        }
    }
}

void PlayerController::handleJumpInput(uint8_t inputState)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    if (inputState & GameInputStateFlag_Jumping && _entity->isGrounded())
    {
        if (state != State_Jumping)
        {
            _entity->setVelocity(b2Vec2(_entity->getVelocity().x, 0));
            stateTime = 0;
            state = State_Jumping;
        }
        
        float vertForce = 0;
        if (state == State_Jumping)
        {
            vertForce = _entity->getBody()->GetMass() * (GM_CFG->_maxRobotVelocityY - stateTime * 0.5f);
            vertForce = clamp(vertForce, 0, _entity->getBody()->GetMass() * GM_CFG->_maxRobotVelocityY);
            _entity->getBody()->ApplyLinearImpulse(b2Vec2(0, vertForce), _entity->getBody()->GetWorldCenter(), true);
        }
        
        if (stateTime > 18 || vertForce <= 0)
        {
            state = State_JumpCompleted;
        }
    }
    else if (state == State_Jumping)
    {
        state = State_JumpCompleted;
    }
}

void PlayerController::handleJumpCompletedInput(uint8_t inputState)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    if (_entity->isGrounded() && stateTime > 18)
    {
        state = State_Idle;
    }
}

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>

IMPL_RTTI(PlayerNetworkController, EntityNetworkController);

IMPL_EntityNetworkController_create(PlayerNetworkController);

PlayerNetworkController::PlayerNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<PlayerController*>(e->getController())), _isLocalPlayer(false)
{
    // Empty
}

PlayerNetworkController::~PlayerNetworkController()
{
    // Empty
}

void PlayerNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->getStateNetworkCache().state;
    
    EntityNetworkController::read(ip);
    
    PlayerController& c = *_controller;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._playerInfo.addressHash);
        ip.read<uint8_t, 3>(c._playerInfo.playerId);
        ip.readSmall(c._playerInfo.playerName);
        
        _isLocalPlayer = NG_CLIENT->isPlayerIdLocal(c._playerInfo.playerId);
        
        c._playerInfoNetworkCache = c._playerInfo;
    }
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._stats.health);
        ip.read(c._stats.target);
        
        c._statsNetworkCache = c._stats;
    }
    
    if (!_isLocalPlayer)
    {
        Util::handleSound(_entity, fromState, _entity->getState().state);
    }
}

uint16_t PlayerNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    PlayerController& c = *_controller;
    
    bool playerInfo = dirtyState & PlayerController::ReadStateFlag_PlayerInfo;
    op.write(playerInfo);
    if (playerInfo)
    {
        op.write(c._playerInfo.addressHash);
        op.write<uint8_t, 3>(c._playerInfo.playerId);
        op.writeSmall(c._playerInfo.playerName);
        
        writtenState |= PlayerController::ReadStateFlag_PlayerInfo;
    }
    
    bool stats = dirtyState & PlayerController::ReadStateFlag_Stats;
    op.write(stats);
    if (stats)
    {
        op.write(c._stats.health);
        op.write(c._stats.target);
        
        writtenState |= PlayerController::ReadStateFlag_Stats;
    }
    
    return writtenState;
}

void PlayerNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    PlayerController& c = *_controller;
    
    c._playerInfo = c._playerInfoNetworkCache;
    c._stats = c._statsNetworkCache;
}

uint16_t PlayerNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    PlayerController& c = *_controller;
    
    if (c._playerInfoNetworkCache != c._playerInfo)
    {
        c._playerInfoNetworkCache = c._playerInfo;
        ret |= PlayerController::ReadStateFlag_PlayerInfo;
    }
    
    if (c._statsNetworkCache != c._stats)
    {
        c._statsNetworkCache = c._stats;
        ret |= PlayerController::ReadStateFlag_Stats;
    }
    
    return ret;
}

bool PlayerNetworkController::isLocalPlayer()
{
    return _isLocalPlayer;
}
