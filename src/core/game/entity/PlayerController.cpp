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
_isLocalPlayer(false),
_isPendingInput(false)
{
    // Empty
}

PlayerController::~PlayerController()
{
    // Empty
}

void PlayerController::update()
{
    if (_entity->isServer())
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
}

void PlayerController::receiveMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case ENTITY_MESSAGE_DAMAGE:
        {
            uint32_t* damageP = static_cast<uint32_t*>(data);
            uint32_t& damage = *damageP;
            damage = clamp(damage, _stats.health, 0);
            _stats.health -= damage;
            if (_stats.health == 0)
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
    // Empty
}

void PlayerController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void PlayerController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_playerInfo.addressHash);
        inInputStream.read<uint8_t, 3>(_playerInfo.playerId);
        inInputStream.read(_playerInfo.playerName);
        
        _isLocalPlayer = NG_CLIENT->isPlayerIdLocal(_playerInfo.playerId);
        
        inReadState |= ReadStateFlag_PlayerInfo;
        _playerInfoCache = _playerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        
        inReadState |= ReadStateFlag_Stats;
        _statsCache = _stats;
    }
    
    if (!isLocalPlayer())
    {
        if (inReadState & Entity::ReadStateFlag_Pose)
        {
            uint8_t fromState = _entity->getPoseCache().state;
            uint8_t toState = _entity->getPose().state;
            
            handleSound(fromState, toState);
        }
    }
}

void PlayerController::recallLastReadState(uint16_t& inReadState)
{
    if (inReadState & ReadStateFlag_PlayerInfo)
    {
        _playerInfo = _playerInfoCache;
    }
    
    if (inReadState & ReadStateFlag_Stats)
    {
        _stats = _statsCache;
    }
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
        inOutputStream.write(_playerInfo.playerName);
        
        writtenState |= ReadStateFlag_PlayerInfo;
    }
    
    bool stats = inDirtyState & ReadStateFlag_Stats;
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_stats.health);
        
        writtenState |= ReadStateFlag_Stats;
    }
    
    return writtenState;
}

void PlayerController::processInput(InputState* inInputState, bool isPending)
{
    GameInputState* is = static_cast<GameInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    GameInputState::PlayerInputState* playerInputState = is->getPlayerInputStateForID(playerId);
    if (playerInputState == NULL)
    {
        return;
    }
    
    _isPendingInput = isPending;
    uint8_t state = _entity->getPose().state;
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
    
    if (isPending)
    {
        handleSound(state, _entity->getPose().state);
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

uint8_t PlayerController::getHealth()
{
    return _stats.health;
}

bool PlayerController::isLocalPlayer()
{
    return _isLocalPlayer;
}

void PlayerController::processInputForIdleState(uint8_t inputState)
{
    handleMovementInput(inputState);
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleMainActionInput(inputState);
    }
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForFirstPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getPose().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForSecondPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getPose().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForThirdPunchState(uint8_t inputState)
{
    handleMainActionInput(inputState);
    
    if (_entity->getPose().state == State_Idle)
    {
        handleMovementInput(inputState);
    }
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleJumpInput(inputState);
    }
}

void PlayerController::processInputForRunningState(uint8_t inputState)
{
    handleMovementInput(inputState);
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
    {
        handleMainActionInput(inputState);
    }
    
    if (_entity->getPose().state == State_Idle ||
        _entity->getPose().state == State_Running)
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
    
    _entity->getPose().state = State_Idle;
    int moveState = STOP;
    if (_entity->isGrounded())
    {
        if (inputState & GameInputStateFlags_MovingRight)
        {
            _entity->getPose().state = State_Running;
            moveState = RIGHT;
        }
        else if (inputState & GameInputStateFlags_MovingLeft)
        {
            _entity->getPose().state = State_Running;
            moveState = LEFT;
        }
    }
    else
    {
        _entity->getPose().state = State_JumpCompleted;
    }
    
    const b2Vec2& vel = _entity->getVelocity();
    float desiredVel = 0;
    switch (moveState)
    {
        case LEFT:
            desiredVel = b2Max(vel.x - 1, -GM_CFG->_maxXVelocity);
            break;
        case STOP:
            desiredVel = vel.x * 0.99f;
            break;
        case RIGHT:
            desiredVel = b2Min(vel.x + 1, GM_CFG->_maxXVelocity);
            break;
    }
    
    float velChange = desiredVel - vel.x;
    float impulse = _entity->getBody()->GetMass() * velChange;
    
    _entity->getPose().isFacingLeft = moveState == LEFT ? true : moveState == RIGHT ? false : _entity->getPose().isFacingLeft;
    
    if (!isCloseEnough(impulse, 0))
    {
        _entity->getBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), _entity->getBody()->GetWorldCenter(), true);
    }
}

void PlayerController::handleMainActionInput(uint8_t inputState)
{
    bool isFirstPunch = _entity->getPose().state == State_FirstPunch;
    bool isSecondPunch = _entity->getPose().state == State_SecondPunch;
    bool isThirdPunch = _entity->getPose().state == State_ThirdPunch;
    if (inputState & GameInputStateFlags_MainAction && _entity->isGrounded())
    {
        if (_entity->getPose().stateFlags == 0 && !isFirstPunch && !isSecondPunch && !isThirdPunch)
        {
            _entity->getPose().state = State_FirstPunch;
            _entity->getPose().stateTime = 0;
            _entity->getPose().stateFlags = 0;
        }
        else if (_entity->getPose().stateFlags == MainActionFlags_ReadyForSecondPunch &&
                 _entity->getPose().stateTime < 18 &&
                 !isSecondPunch)
        {
            _entity->getPose().stateFlags = MainActionFlags_ToThrowSecondPunch;
        }
        else if (_entity->getPose().stateFlags == MainActionFlags_ReadyForThirdPunch &&
                 _entity->getPose().stateTime < 18 &&
                 !isThirdPunch)
        {
            _entity->getPose().stateFlags = MainActionFlags_ToThrowThirdPunch;
        }
        
        if (_entity->getPose().stateFlags == MainActionFlags_ToThrowSecondPunch &&
            _entity->getPose().stateTime > 18)
        {
            _entity->getPose().state = State_SecondPunch;
            _entity->getPose().stateTime = 0;
            _entity->getPose().stateFlags = 0;
        }
        else if (_entity->getPose().stateFlags == MainActionFlags_ToThrowThirdPunch &&
                 _entity->getPose().stateTime > 18)
        {
            _entity->getPose().state = State_ThirdPunch;
            _entity->getPose().stateTime = 0;
            _entity->getPose().stateFlags = 0;
        }
    }
    else
    {
        if (_entity->isGrounded())
        {
            if (isFirstPunch)
            {
                if (_entity->getPose().stateFlags == 0)
                {
                    _entity->getPose().stateFlags = MainActionFlags_ReadyForSecondPunch;
                }
            }
            else if (isSecondPunch)
            {
                if (_entity->getPose().stateFlags == 0)
                {
                    _entity->getPose().stateFlags = MainActionFlags_ReadyForThirdPunch;
                }
            }
            else if (isThirdPunch)
            {
                _entity->getPose().stateFlags = 0;
            }
        }
        else if (isFirstPunch || isSecondPunch || isThirdPunch)
        {
            _entity->getPose().state = State_Idle;
            _entity->getPose().stateFlags = 0;
        }
        
        if (_entity->getPose().stateFlags == MainActionFlags_ToThrowSecondPunch &&
            _entity->getPose().stateTime > 18)
        {
            _entity->getPose().state = State_SecondPunch;
            _entity->getPose().stateTime = 0;
            _entity->getPose().stateFlags = 0;
        }
        else if (_entity->getPose().stateFlags == MainActionFlags_ToThrowThirdPunch &&
                 _entity->getPose().stateTime > 18)
        {
            _entity->getPose().state = State_ThirdPunch;
            _entity->getPose().stateTime = 0;
            _entity->getPose().stateFlags = 0;
        }
        
        if ((isFirstPunch && _entity->getPose().stateTime > 18) ||
            (isSecondPunch && _entity->getPose().stateTime > 18) ||
            (isThirdPunch && _entity->getPose().stateTime > 30))
        {
            _entity->getPose().state = State_Idle;
            _entity->getPose().stateFlags = 0;
        }
    }
}

void PlayerController::handleJumpInput(uint8_t inputState)
{
    bool isJumping = _entity->getPose().state == State_Jumping;
    if (inputState & GameInputStateFlags_Jumping && _entity->isGrounded())
    {
        if (!isJumping)
        {
            _entity->setVelocity(b2Vec2(_entity->getVelocity().x, 0));
            _entity->getPose().stateTime = 0;
            _entity->getPose().state = State_Jumping;
        }
        
        float vertForce = 0;
        if (_entity->getPose().state == State_Jumping)
        {
            vertForce = _entity->getBody()->GetMass() * (GM_CFG->_maxYVelocity - _entity->getPose().stateTime * 0.5f);
            vertForce = clamp(vertForce, _entity->getBody()->GetMass() * GM_CFG->_maxYVelocity, 0);
            _entity->getBody()->ApplyLinearImpulse(b2Vec2(0, vertForce), _entity->getBody()->GetWorldCenter(), true);
        }
        
        if (_entity->getPose().stateTime > 18 || vertForce <= 0)
        {
            _entity->getPose().state = State_JumpCompleted;
        }
    }
    else if (isJumping)
    {
        _entity->getPose().state = State_JumpCompleted;
    }
}

void PlayerController::handleJumpCompletedInput(uint8_t inputState)
{
    if (_entity->isGrounded() && _entity->getPose().stateTime > 18)
    {
        _entity->getPose().state = State_Idle;
    }
}

void PlayerController::handleSound(uint8_t fromState, uint8_t toState)
{
    if (fromState == toState)
    {
        return;
    }
    
    switch (toState)
    {
        case State_Jumping:
            GM_UTIL->playSound(_entity->getSoundMapping(State_Jumping), _entity->getPosition());
            break;
        default:
            break;
    }
}
