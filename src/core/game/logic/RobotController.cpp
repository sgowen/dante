//
//  RobotController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/RobotController.h>

#include <framework/entity/Entity.h>
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "Box2D/Box2D.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/Move.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/GameInputManager.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/Util.h"
#include "game/logic/Server.h"
#include <framework/util/FlagUtil.h>

NGRTTI_IMPL(RobotController, EntityController);

#define SPEED 8.0f
#define SPRINT_SPEED SPEED * 1.5f
#define MOVEMENT_FORCE SPEED * 6
#define SPRINT_MOVEMENT_FORCE MOVEMENT_FORCE * 1.5f

EntityController* RobotController::create(Entity* inEntity)
{
    return new RobotController(inEntity);
}

RobotController::RobotController(Entity* inEntity) : EntityController(inEntity),
_playerInfo(),
_playerInfoCache(_playerInfo),
_stats(),
_statsCache(_stats),
_isLocalPlayer(false)
{
    _stateMappings.insert(std::make_pair(0, "Idle"));
    _stateMappings.insert(std::make_pair(1, "Punching"));
    _stateMappings.insert(std::make_pair(2, "Running"));
    _stateMappings.insert(std::make_pair(3, "Sprinting"));
    _stateMappings.insert(std::make_pair(4, "Jumping"));
}

RobotController::~RobotController()
{
    // Empty
}

uint8_t RobotController::update()
{
    if (_stats.health == 0)
    {
        _entity->requestDeletion();
    }
    
    if (_entity->isServer())
    {
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
    
    bool isMoving = _entity->getVelocity().x < -0.5f || _entity->getVelocity().x > 0.5f;
    return _entity->isGrounded() && getNumJumps() == 0 ? isMainAction() ? State_Punching : !isMoving ? State_Idle : isSprinting() ? State_Running_Fast : State_Running : State_Jumping;
}

bool RobotController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Don't collide with other players
    return inEntity->getEntityDef().type != _entity->getEntityDef().type;
}

void RobotController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void RobotController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void RobotController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_playerInfo.addressHash);
        inInputStream.read<uint8_t, 3>(_playerInfo.playerId);
        inInputStream.read(_playerInfo.playerName);
        
        _isLocalPlayer = NG_CLIENT->isPlayerIdLocal(_playerInfo.playerId);
        
        setFlag(inReadState, ReadStateFlag_PlayerInfo);
        _playerInfoCache = _playerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        
        setFlag(inReadState, ReadStateFlag_Stats);
        _statsCache = _stats;
    }
}

void RobotController::recallLastReadState(uint16_t& inReadState)
{
    if (isFlagSet(inReadState, ReadStateFlag_PlayerInfo))
    {
        _playerInfo = _playerInfoCache;
    }
    
    if (isFlagSet(inReadState, ReadStateFlag_Stats))
    {
        _stats = _statsCache;
    }
}

uint16_t RobotController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    uint16_t writtenState = inWrittenState;
    
    bool playerInfo = isFlagSet(inDirtyState, ReadStateFlag_PlayerInfo);
    inOutputStream.write(playerInfo);
    if (playerInfo)
    {
        inOutputStream.write(_playerInfo.addressHash);
        inOutputStream.write<uint8_t, 3>(_playerInfo.playerId);
        inOutputStream.write(_playerInfo.playerName);
        
        setFlag(writtenState, ReadStateFlag_PlayerInfo);
    }
    
    bool stats = isFlagSet(inDirtyState, ReadStateFlag_Stats);
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_stats.health);
        
        setFlag(writtenState, ReadStateFlag_Stats);
    }
    
    return writtenState;
}

void RobotController::processInput(InputState* inInputState, bool isPending)
{
    MainInputState* is = static_cast<MainInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    MainInputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == NULL)
    {
        return;
    }
    
    const b2Vec2& velocity = _entity->getVelocity();
    
    float sideForce = 0;
    float vertForce = 0;
    
    bool isSprinting = inputState->isSprinting();
    if (isSprinting)
    {
        if (!isFlagSet(_entity->getPose().state, StateFlag_Sprinting))
        {
            setFlag(_entity->getPose().state, StateFlag_Sprinting);
            
            if (isPending)
            {
                Util::playSound(SOUND_ID_ACTIVATE_SPRINT, _entity->getPosition());
            }
        }
        
        if ((inputState->getDesiredRightAmount() > 0 && velocity.x < SPRINT_SPEED)
            || (inputState->getDesiredRightAmount() < 0 && velocity.x > -SPRINT_SPEED))
        {
            sideForce = inputState->getDesiredRightAmount() * SPRINT_MOVEMENT_FORCE;
        }
    }
    else
    {
        removeFlag(_entity->getPose().state, StateFlag_Sprinting);
        
        if ((inputState->getDesiredRightAmount() > 0 && velocity.x < SPEED)
            || (inputState->getDesiredRightAmount() < 0 && velocity.x > -SPEED))
        {
            sideForce = inputState->getDesiredRightAmount() * MOVEMENT_FORCE;
        }
    }
    
    if (inputState->isJumping())
    {
        if (_entity->isGrounded() && getNumJumps() == 0)
        {
            _entity->setVelocity(b2Vec2(velocity.x, 0));
            _entity->getPose().stateTime = 0;
            removeFlag(_entity->getPose().state, StateFlag_FirstJumpCompleted);
            removeFlag(_entity->getPose().state, StateFlag_SecondJump);
            setFlag(_entity->getPose().state, StateFlag_FirstJump);
            
            if (isPending)
            {
                Util::playSound(SOUND_ID_ROBOT_JUMP, _entity->getPosition());
            }
        }
        else if (getNumJumps() == 0)
        {
            removeFlag(_entity->getPose().state, StateFlag_SecondJump);
            setFlag(_entity->getPose().state, StateFlag_FirstJump);
            setFlag(_entity->getPose().state, StateFlag_FirstJumpCompleted);
        }
        
        if (getNumJumps() == 1)
        {
            if (isFlagSet(_entity->getPose().state, StateFlag_FirstJumpCompleted))
            {
                _entity->getPose().stateTime = 0;
                removeFlag(_entity->getPose().state, StateFlag_FirstJump);
                setFlag(_entity->getPose().state, StateFlag_SecondJump);
                _entity->setVelocity(b2Vec2(velocity.x, 0));
                
                if (isPending)
                {
                    Util::playSound(SOUND_ID_ROBOT_JUMP, _entity->getPosition());
                }
            }
            else
            {
                vertForce = _entity->getBody()->GetMass() * (11.25f - (7.0f + (_entity->getPose().stateTime + 0.25f) * 10));
            }
        }
        
        if (getNumJumps() == 2)
        {
            vertForce = _entity->getBody()->GetMass() * (10.25f - (6.0f + (_entity->getPose().stateTime + 0.25f) * 10));
        }
        
        vertForce = clamp(vertForce, _entity->getBody()->GetMass() * 8.0f, 0);
    }
    else
    {
        if (!isFlagSet(_entity->getPose().state, StateFlag_FirstJumpCompleted))
        {
            setFlag(_entity->getPose().state, StateFlag_FirstJumpCompleted);
        }
    }
    
    if (_entity->isGrounded() && _entity->getPose().stateTime > 0.3f)
    {
        removeFlag(_entity->getPose().state, StateFlag_FirstJumpCompleted);
        removeFlag(_entity->getPose().state, StateFlag_FirstJump);
        removeFlag(_entity->getPose().state, StateFlag_SecondJump);
    }
    
    _entity->getBody()->ApplyLinearImpulse(b2Vec2(sideForce,vertForce), _entity->getBody()->GetWorldCenter(), true);
    
    _entity->getPose().isFacingLeft = sideForce < 0 ? true : sideForce > 0 ? false : _entity->getPose().isFacingLeft;
    
    if (inputState->isMainAction())
    {
        setFlag(_entity->getPose().state, StateFlag_MainAction);
    }
    else
    {
        removeFlag(_entity->getPose().state, StateFlag_MainAction);
    }
}

void RobotController::setAddressHash(uint64_t addressHash)
{
    _playerInfo.addressHash = addressHash;
}

uint64_t RobotController::getAddressHash() const
{
    return _playerInfo.addressHash;
}

void RobotController::setPlayerId(uint8_t inPlayerId)
{
    _playerInfo.playerId = inPlayerId;
}

uint8_t RobotController::getPlayerId() const
{
    return _playerInfo.playerId;
}

void RobotController::setPlayerName(std::string playerName)
{
    _playerInfo.playerName = playerName;
}

std::string& RobotController::getPlayerName()
{
    return _playerInfo.playerName;
}

uint8_t RobotController::getHealth()
{
    return _stats.health;
}

uint8_t RobotController::getNumJumps()
{
    if (isFlagSet(_entity->getPose().state, StateFlag_FirstJump))
    {
        return 1;
    }
    else if (isFlagSet(_entity->getPose().state, StateFlag_SecondJump))
    {
        return 2;
    }
    
    return 0;
}

bool RobotController::isMainAction()
{
    return isFlagSet(_entity->getPose().state, StateFlag_MainAction);
}

bool RobotController::isSprinting()
{
    return isFlagSet(_entity->getPose().state, StateFlag_Sprinting);
}

bool RobotController::isLocalPlayer()
{
    return _isLocalPlayer;
}
