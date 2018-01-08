//
//  RobotController.cpp
//  noctisgames-framework
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
_statsCache(_stats)
{
    // Empty
}

RobotController::~RobotController()
{
    // Empty
}

void RobotController::update()
{
    if (_entity->_isServer)
    {
        if (_stats.health == 0)
        {
            _entity->requestDeletion();
        }
        
        if (_entity->isRequestingDeletion())
        {
            Server::sHandleNewClient(_playerInfo.playerId, _playerInfo.playerName);
        }
        
        if (_playerInfoCache != _playerInfo)
        {
            NG_SERVER->setStateDirty(_entity->getID(), ROBT_PlayerInfo);
        }
        _playerInfoCache = _playerInfo;
        
        if (_statsCache != _stats)
        {
            NG_SERVER->setStateDirty(_entity->getID(), ROBT_Stats);
        }
        _statsCache = _stats;
    }
}

bool RobotController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return inEntity->getEntityDef().type != _entity->_entityDef.type;
}

void RobotController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void RobotController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void RobotController::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_playerInfo.addressHash);
        inInputStream.read(_playerInfo.playerId);
        inInputStream.read(_playerInfo.playerName);
        
        setFlag(_entity->_readState, ROBT_PlayerInfo);
        
        _playerInfoCache = _playerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        
        setFlag(_entity->_readState, ROBT_Stats);
        
        _statsCache = _stats;
    }
}

uint16_t RobotController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    uint16_t writtenState = inWrittenState;
    
    bool playerInfo = isFlagSet(inDirtyState, ROBT_PlayerInfo);
    inOutputStream.write(playerInfo);
    if (playerInfo)
    {
        inOutputStream.write(_playerInfo.addressHash);
        inOutputStream.write(_playerInfo.playerId);
        inOutputStream.write(_playerInfo.playerName);
        
        setFlag(writtenState, ROBT_PlayerInfo);
    }
    
    bool stats = isFlagSet(inDirtyState, ROBT_Stats);
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_stats.health);
        
        setFlag(writtenState, ROBT_Stats);
    }
    
    return writtenState;
}

void RobotController::recallLastReadState()
{
    if (isFlagSet(_entity->_readState, ROBT_PlayerInfo))
    {
        _playerInfo = _playerInfoCache;
    }
    
    if (isFlagSet(_entity->_readState, ROBT_Stats))
    {
        _stats = _statsCache;
    }
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
        if (!isFlagSet(_entity->_pose.state, ROBT_SPRINTING))
        {
            setFlag(_entity->_pose.state, ROBT_SPRINTING);
            
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
        removeFlag(_entity->_pose.state, ROBT_SPRINTING);
        
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
            _entity->_pose.stateTime = 0;
            removeFlag(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED);
            removeFlag(_entity->_pose.state, ROBT_SECOND_JUMP);
            setFlag(_entity->_pose.state, ROBT_FIRST_JUMP);
            
            if (isPending)
            {
                Util::playSound(SOUND_ID_ROBOT_JUMP, _entity->getPosition());
            }
        }
        else if (getNumJumps() == 0)
        {
            removeFlag(_entity->_pose.state, ROBT_SECOND_JUMP);
            setFlag(_entity->_pose.state, ROBT_FIRST_JUMP);
            setFlag(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED);
        }
        
        if (getNumJumps() == 1)
        {
            if (isFlagSet(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED))
            {
                _entity->_pose.stateTime = 0;
                removeFlag(_entity->_pose.state, ROBT_FIRST_JUMP);
                setFlag(_entity->_pose.state, ROBT_SECOND_JUMP);
                _entity->setVelocity(b2Vec2(velocity.x, 0));
                
                if (isPending)
                {
                    Util::playSound(SOUND_ID_ROBOT_JUMP, _entity->getPosition());
                }
            }
            else
            {
                vertForce = _entity->_body->GetMass() * (11.25f - (7.0f + (_entity->_pose.stateTime + 0.25f) * 10));
            }
        }
        
        if (getNumJumps() == 2)
        {
            vertForce = _entity->_body->GetMass() * (10.25f - (6.0f + (_entity->_pose.stateTime + 0.25f) * 10));
        }
        
        vertForce = clamp(vertForce, _entity->_body->GetMass() * 8.0f, 0);
    }
    else
    {
        if (!isFlagSet(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED))
        {
            setFlag(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED);
        }
    }
    
    if (_entity->isGrounded() && _entity->_pose.stateTime > 0.3f)
    {
        removeFlag(_entity->_pose.state, ROBT_FIRST_JUMP_COMPLETED);
        removeFlag(_entity->_pose.state, ROBT_FIRST_JUMP);
        removeFlag(_entity->_pose.state, ROBT_SECOND_JUMP);
    }
    
    _entity->_body->ApplyLinearImpulse(b2Vec2(sideForce,vertForce), _entity->_body->GetWorldCenter(), true);
    
    if (sideForce < 0)
    {
        setFlag(_entity->_pose.state, ROBT_FACING_LEFT);
    }
    else if (sideForce > 0)
    {
        removeFlag(_entity->_pose.state, ROBT_FACING_LEFT);
    }
    
    if (inputState->isMainAction())
    {
        setFlag(_entity->_pose.state, ROBT_MAIN_ACTION);
    }
    else
    {
        removeFlag(_entity->_pose.state, ROBT_MAIN_ACTION);
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
    if (isFlagSet(_entity->_pose.state, ROBT_FIRST_JUMP))
    {
        return 1;
    }
    else if (isFlagSet(_entity->_pose.state, ROBT_SECOND_JUMP))
    {
        return 2;
    }
    
    return 0;
}

bool RobotController::isFacingLeft()
{
    return isFlagSet(_entity->_pose.state, ROBT_FACING_LEFT);
}

bool RobotController::isMainAction()
{
    return isFlagSet(_entity->_pose.state, ROBT_MAIN_ACTION);
}

bool RobotController::isSprinting()
{
    return isFlagSet(_entity->_pose.state, ROBT_SPRINTING);
}
