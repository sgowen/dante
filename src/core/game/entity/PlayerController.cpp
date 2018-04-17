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
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <Box2D/Box2D.h>
#include <game/game/GameInputState.h>
#include <framework/network/portable/Move.h>

#include <game/logic/World.h>
#include <framework/util/macros.h>
#include <framework/util/Constants.h>
#include <framework/util/Timing.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <game/game/GameInputManager.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/logic/Util.h>
#include <game/logic/Server.h>
#include <framework/util/Config.h>
#include <game/logic/GameConfig.h>

NGRTTI_IMPL(PlayerController, EntityController);

EntityController* PlayerController::create(Entity* inEntity)
{
    return new PlayerController(inEntity);
}

PlayerController::PlayerController(Entity* inEntity) : EntityController(inEntity),
_playerInfo(),
_playerInfoCache(_playerInfo),
_stats(),
_statsCache(_stats),
_attackSensorFixture(NULL),
_isLocalPlayer(false)
{
    // Empty
}

PlayerController::~PlayerController()
{
    // Empty
}

void PlayerController::update(bool isLive)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    if ((state == State_FirstPunch && stateTime == 1) ||
        (state == State_SecondPunch && stateTime == 1) ||
        (state == State_ThirdPunch && stateTime == 1))
    {
        if (_stats.target > 0)
        {
            EntityManager* entityManager = _entity->isServer() ? NG_SERVER->getEntityManager() : NG_CLIENT->getEntityManager();
            Entity* e = entityManager->getEntityByID(_stats.target);
            if (e)
            {
                uint32_t damage = state == State_ThirdPunch ? 2 : 1;
                e->getController()->receiveMessage(ENTITY_MESSAGE_DAMAGE, isLive, &damage);
            }
        }
    }
}

void PlayerController::postUpdate()
{
    if (_stats.health == 0)
    {
        _entity->requestDeletion();
    }
    
    if (_playerInfoCache != _playerInfo)
    {
        _playerInfoCache = _playerInfo;
        NG_SERVER->setStateDirty(_entity->getID(), ReadStateFlag_PlayerInfo);
    }
    
    if (_statsCache != _stats)
    {
        _statsCache = _stats;
        NG_SERVER->setStateDirty(_entity->getID(), ReadStateFlag_Stats);
    }
}

void PlayerController::receiveMessage(uint16_t message, bool isLive, void* data)
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
            if (health == 0)
            {
                /// TODO
            }
        }
            break;
        default:
            break;
    }
}

void PlayerController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    assert(fixtures.size() == 4);
    _attackSensorFixture = fixtures[3];
    assert(_attackSensorFixture);
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

void PlayerController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_playerInfo.addressHash);
        inInputStream.read<uint8_t, 3>(_playerInfo.playerId);
        inInputStream.readSmall(_playerInfo.playerName);
        
        _isLocalPlayer = NG_CLIENT->isPlayerIdLocal(_playerInfo.playerId);
        
        inReadState |= ReadStateFlag_PlayerInfo;
        _playerInfoCache = _playerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        inInputStream.read(_stats.target);
        
        inReadState |= ReadStateFlag_Stats;
        _statsCache = _stats;
    }
    
    if (!isLocalPlayer())
    {
        Util::handleSound(_entity, _entity->getStateCache().state, _entity->getState().state);
    }
}

void PlayerController::recallCache(uint16_t& inReadState)
{
    _playerInfo = _playerInfoCache;
    _stats = _statsCache;
}

uint16_t PlayerController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    uint16_t writtenState = inWrittenState;
    
    bool playerInfo = inDirtyState & ReadStateFlag_PlayerInfo;
    inOutputStream.write(playerInfo);
    if (playerInfo)
    {
        inOutputStream.write(_playerInfo.addressHash);
        inOutputStream.write<uint8_t, 3>(_playerInfo.playerId);
        inOutputStream.writeSmall(_playerInfo.playerName);
        
        writtenState |= ReadStateFlag_PlayerInfo;
    }
    
    bool stats = inDirtyState & ReadStateFlag_Stats;
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_stats.health);
        inOutputStream.write(_stats.target);
        
        writtenState |= ReadStateFlag_Stats;
    }
    
    return writtenState;
}

void PlayerController::processInput(InputState* inInputState, bool isLive)
{
    GameInputState* is = static_cast<GameInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    GameInputState::PlayerInputState* playerInputState = is->getPlayerInputStateForID(playerId);
    if (playerInputState == NULL)
    {
        return;
    }
    
    uint8_t state = _entity->getState().state;
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
    
    if (isLive)
    {
        Util::handleSound(_entity, state, _entity->getState().state);
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

bool PlayerController::isLocalPlayer()
{
    return _isLocalPlayer;
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
