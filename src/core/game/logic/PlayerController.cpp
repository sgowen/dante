//
//  PlayerController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/PlayerController.h>

#include <framework/entity/Entity.h>
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "Box2D/Box2D.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/Move.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "framework/util/Constants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/util/MathUtil.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/GameInputManager.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/Util.h"
#include "game/logic/Server.h"
#include "framework/util/Config.h"
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

uint8_t PlayerController::update()
{
    if (_entity->isServer())
    {
        if (_stats.health == 0)
        {
            _entity->requestDeletion();
        }
        
        if (_entity->isRequestingDeletion())
        {
            // Respawn
            Server::sHandleNewClient(_playerInfo.playerId, _playerInfo.playerName);
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
    
    bool isGrounded = _entity->isGrounded();
    uint8_t numJumps = getNumJumps();
    bool mainAction = isMainAction();
    bool moving = isMoving();
    
    return isGrounded && numJumps == 0 ?
                mainAction ?
                    State_Punching :
                !moving ?
                    State_Idle :
                State_Running :
            State_Jumping;
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
    assert(fixtures.size() == 3);
    _attackSensorFixture = fixtures[2];
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
            uint8_t& oldState = _entity->getPoseCache().state;
            uint8_t& newState = _entity->getPose().state;
            
            if ((!(oldState & StateFlag_FirstJump) &&
                (newState & StateFlag_FirstJump)) ||
                (!(oldState & StateFlag_SecondJump) &&
                 (newState & StateFlag_SecondJump)))
            {
                Util::playSound(_entity->getSoundMapping(State_Jumping), _entity->getPosition());
            }
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
    MainInputState* is = static_cast<MainInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    MainInputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == NULL)
    {
        return;
    }
    
    const b2Vec2& velocity = _entity->getVelocity();
    
    float vertForce = 0;
    
    bool isDoubleJumpFrame = false;
    if (inputState->isJumping())
    {
        if (_entity->isGrounded())
        {
            if (getNumJumps() == 0)
            {
                _entity->setVelocity(b2Vec2(velocity.x, 0));
                _entity->getPose().stateTime = 0;
                _entity->getPose().state &= ~StateFlag_FirstJumpCompleted;
                _entity->getPose().state &= ~StateFlag_SecondJump;
                _entity->getPose().state |= StateFlag_FirstJump;
                
                if (isPending)
                {
                    Util::playSound(_entity->getSoundMapping(State_Jumping), _entity->getPosition());
                }
            }
        }
        else if (getNumJumps() == 0)
        {
            _entity->getPose().state &= ~StateFlag_SecondJump;
            _entity->getPose().state |= StateFlag_FirstJump;
            _entity->getPose().state |= StateFlag_FirstJumpCompleted;
        }
        
        if (getNumJumps() == 1)
        {
            if (_entity->getPose().state & StateFlag_FirstJumpCompleted)
            {
                _entity->getPose().stateTime = 0;
                _entity->getPose().state &= ~StateFlag_FirstJump;
                _entity->getPose().state |= StateFlag_SecondJump;
                _entity->setVelocity(b2Vec2(velocity.x, 0));
                
                if (isPending)
                {
                    Util::playSound(_entity->getSoundMapping(State_Jumping), _entity->getPosition());
                }
            }
            else
            {
                vertForce = _entity->getBody()->GetMass() * (GM_CFG->_maxYVelocity - _entity->getPose().stateTime * 0.5f);
            }
        }
        
        if (getNumJumps() == 2)
        {
            vertForce = _entity->getBody()->GetMass() * (GM_CFG->_maxYVelocity * 0.75f - _entity->getPose().stateTime * 0.35f);
        }
        
        vertForce = clamp(vertForce, _entity->getBody()->GetMass() * GM_CFG->_maxYVelocity, 0);
    }
    else
    {
        bool wasFirstJumpCompleted = _entity->getPose().state & StateFlag_FirstJumpCompleted;
        if (!wasFirstJumpCompleted)
        {
            _entity->getPose().state |= StateFlag_FirstJumpCompleted;
        }
    }
    
    if (_entity->isGrounded() && _entity->getPose().stateTime >= 12)
    {
        _entity->getPose().state &= ~StateFlag_FirstJumpCompleted;
        _entity->getPose().state &= ~StateFlag_FirstJump;
        _entity->getPose().state &= ~StateFlag_SecondJump;
    }
    
    float right = inputState->getDesiredRightAmount();
    bool isRight = right > 0;
    bool isLeft = right < 0;
    
    bool wasMainAction = _entity->getPose().state & StateFlag_MainAction;
    bool isMainAction = false;
    if (inputState->isMainAction() && _entity->isGrounded())
    {
        isMainAction = true;
        _entity->getPose().state |= StateFlag_MainAction;
    }
    else
    {
        isMainAction = false;
        _entity->getPose().state &= ~StateFlag_MainAction;
    }
    
    if (isMainAction && !wasMainAction)
    {
        _entity->getPose().stateTime = 0;
    }
    
    static const int MS_LEFT = 0;
    static const int MS_STOP = 1;
    static const int MS_RIGHT = 2;
    
    const b2Vec2& vel = velocity;
    if (getNumJumps() == 2 && _entity->getPose().stateTime < 12)
    {
        isDoubleJumpFrame = true;
    }
    
    int moveState = MS_STOP;
    if (!isMainAction)
    {
        if (_entity->isGrounded() || isDoubleJumpFrame)
        {
            if (isRight)
            {
                moveState = MS_RIGHT;
            }
            else if (isLeft)
            {
                moveState = MS_LEFT;
            }
        }
    }
    
    float desiredVel = 0;
    switch (moveState)
    {
        case MS_LEFT:
            desiredVel = b2Max(vel.x - 1, -GM_CFG->_maxXVelocity);
            isLeft = true;
            break;
        case MS_STOP:
            desiredVel = vel.x * 0.99f;
            isLeft = false;
            isRight = false;
            break;
        case MS_RIGHT:
            desiredVel = b2Min(vel.x + 1, GM_CFG->_maxXVelocity);
            isRight = true;
            break;
    }
    float velChange = desiredVel - vel.x;
    float impulse = _entity->getBody()->GetMass() * velChange;
    
    _entity->getPose().isFacingLeft = isLeft ? true : isRight ? false : _entity->getPose().isFacingLeft;
    
    if (impulse != 0 || vertForce != 0)
    {
        _entity->getBody()->ApplyLinearImpulse(b2Vec2(impulse,vertForce), _entity->getBody()->GetWorldCenter(), true);
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

uint8_t PlayerController::getNumJumps()
{
    if (_entity->getPose().state & StateFlag_FirstJump)
    {
        return 1;
    }
    else if (_entity->getPose().state & StateFlag_SecondJump)
    {
        return 2;
    }
    
    return 0;
}

bool PlayerController::isMainAction()
{
    return _entity->getPose().state & StateFlag_MainAction;
}

bool PlayerController::isMoving()
{
    return !isCloseEnough(_entity->getVelocity().x, 0);
}

bool PlayerController::isLocalPlayer()
{
    return _isLocalPlayer;
}
