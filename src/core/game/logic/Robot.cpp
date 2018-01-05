//
//  Robot.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Robot.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/Move.h"
#include "game/logic/Ground.h"
#include "game/logic/Crate.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/entity/EntityRegistry.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/GameInputManager.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/Util.h"
#include "game/logic/Server.h"
#include "game/logic/Ground.h"

#include <math.h>

NGRTTI_IMPL(Robot, Entity);

NW_TYPE_IMPL(Robot);

namespace
{
    inline EntityDef constructEntityDef()
    {
        EntityDef ret;
        
        ret.isStaticBody = false;
        ret.restitution = 0.1f;
        ret.isCharacter = true;
        ret.density = 16.0f;
        
        return ret;
    }
}

Robot::Robot(b2World& world, bool isServer) : Entity(world, 0, 0, 2.0f, 2.0f, isServer, constructEntityDef()),
_addressHash(0),
_playerId(0),
_playerName("Robot"),
_numJumps(0),
_health(255),
_isFacingLeft(false),
_isMainAction(false),
_isSprinting(false),
_isFirstJumpCompleted(false),
_speed(7.5f),
_movementForce(_speed * 6),
_jumpForce(600.0f),
_doubleJumpForce(400.0f),
_numJumpsLastKnown(0),
_healthLastKnown(_health),
_isFacingLeftLastKnown(false),
_isFirstJumpCompletedLastKnown(false)
{
    // Empty
}

Robot::~Robot()
{
    if (!_isServer)
    {
        NG_AUDIO_ENGINE->playSound(SOUND_ID_DEATH);
    }
}

void Robot::update()
{
    _stateTime += FRAME_RATE;
    
    if (_isServer)
    {
        if (getPosition().y < DEAD_ZONE_BOTTOM)
        {
            _health = 0;
        }
        
        if (_health == 0 && !isRequestingDeletion())
        {
            // TODO, this is NOT the right way to handle the player dying
            
            requestDeletion();
            
            if (_isServer)
            {
                Server::sHandleNewClient(_playerId, _playerName);
            }
        }
        
        NG_SERVER->setStateDirty(getID(), ROBT_Pose);
        
        if (_healthLastKnown != _health)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Stats);
        }
    }
    else
    {
        if (!NG_CLIENT->isPlayerIdLocal(getPlayerId()))
        {
            playNetworkBoundSounds(_numJumpsLastKnown, _isSprintingLastKnown);
        }
    }
    
    _velocityLastKnown.Set(getVelocity().x, getVelocity().y);
    _positionLastKnown.Set(getPosition().x, getPosition().y);
    _numGroundContactsLastKnown = _numGroundContacts;
    _numJumpsLastKnown = _numJumps;
    _healthLastKnown = _health;
    _isFacingLeftLastKnown = _isFacingLeft;
    _isSprintingLastKnown = _isSprinting;
    _isFirstJumpCompletedLastKnown = _isFirstJumpCompleted;
}

bool Robot::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        return inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti);
    }
    
    return inEntity->getRTTI().derivesFrom(Ground::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void Robot::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        _numJumps = 0;
        
        handleBeginFootContact(inEntity);
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        return;
    }
}

void Robot::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        if (inEntity->getRTTI().derivesFrom(Ground::rtti)
            || inEntity->getRTTI().derivesFrom(Crate::rtti))
        {
            handleEndFootContact(inEntity);
        }
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        return;
    }
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    _readState = 0;
    
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_addressHash);
        inInputStream.read(_playerId);
        inInputStream.read(_playerName);
        inInputStream.read(_color);
        
        _readState |= ROBT_PlayerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stateTime);
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        inInputStream.read<uint8_t, 2>(_numJumps);
        inInputStream.read<uint8_t, 2>(_numGroundContacts);
        
        inInputStream.read(_isFacingLeft);
        inInputStream.read(_isMainAction);
        inInputStream.read(_isSprinting);
        inInputStream.read(_isFirstJumpCompleted);
        
        _readState |= ROBT_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_health);
        
        _readState |= ROBT_Stats;
    }
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    bool playerInfo = inDirtyState & ROBT_PlayerInfo;
    inOutputStream.write(playerInfo);
    if (playerInfo)
    {
        inOutputStream.write(_addressHash);
        inOutputStream.write(_playerId);
        inOutputStream.write(_playerName);
        inOutputStream.write(_color);
        
        writtenState |= ROBT_PlayerInfo;
    }
    
    bool pose = inDirtyState & ROBT_Pose;
    inOutputStream.write(pose);
    if (pose)
    {
        inOutputStream.write(_stateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write<uint8_t, 2>(_numJumps);
        inOutputStream.write<uint8_t, 2>(_numGroundContacts);
        
        inOutputStream.write(_isFacingLeft);
        inOutputStream.write(_isMainAction);
        inOutputStream.write(_isSprinting);
        inOutputStream.write(_isFirstJumpCompleted);
        
        writtenState |= ROBT_Pose;
    }
    
    bool stats = inDirtyState & ROBT_Stats;
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_health);
        
        writtenState |= ROBT_Stats;
    }
    
    return writtenState;
}

bool Robot::needsMoveReplay()
{
    return (_readState & ROBT_Pose) != 0;
}

void Robot::processInput(InputState* inInputState, bool isPending)
{
    MainInputState* is = static_cast<MainInputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    MainInputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == NULL)
    {
        return;
    }
    
    uint8_t numJumpsLastKnown = _numJumps;
    bool isSprintingLastKnown = _isSprinting;
    
    const b2Vec2& velocity = getVelocity();
    
    float movementForce = isGrounded() ? _movementForce : 0;
    float sideForce = 0;
    float vertForce = 0;
    _isSprinting = inputState->isSprinting() && isGrounded();
    if (_isSprinting)
    {
        if ((inputState->getDesiredRightAmount() > 0 && velocity.x < (_speed * 1.5f))
            || (inputState->getDesiredRightAmount() < 0 && velocity.x > (-_speed * 1.5f)))
        {
            sideForce = inputState->getDesiredRightAmount() * movementForce * 1.5f;
        }
    }
    else
    {
        if ((inputState->getDesiredRightAmount() > 0 && velocity.x < _speed)
            || (inputState->getDesiredRightAmount() < 0 && velocity.x > -_speed))
        {
            sideForce = inputState->getDesiredRightAmount() * movementForce;
        }
    }
    
    _isFacingLeft = sideForce < 0 ? true : sideForce > 0 ? false : _isFacingLeft;
    
    if (inputState->isJumping())
    {
        if (isGrounded() && _numJumps == 0)
        {
            setVelocity(b2Vec2(velocity.x, 0));
            _stateTime = 0;
            _isFirstJumpCompleted = false;
            _numJumps = 1;
        }
        else if (_numJumps == 0)
        {
            _numJumps = 1;
            _isFirstJumpCompleted = true;
        }
        
        if (_numJumps == 1)
        {
            if (_isFirstJumpCompleted)
            {
                _stateTime = 0;
                _numJumps = 2;
                setVelocity(b2Vec2(velocity.x, 0));
            }
            else
            {
                vertForce = _body->GetMass() * (8.0f - (4.0f + (_stateTime + 0.25f) * 10));
                
                if (isGrounded() && _stateTime > 0.3f)
                {
                    setVelocity(b2Vec2(velocity.x, 0));
                    _stateTime = 0;
                    _isFirstJumpCompleted = false;
                    _numJumps = 0;
                }
            }
        }
        
        if (_numJumps == 2)
        {
            vertForce = _body->GetMass() * (10.0f - (6.0f + (_stateTime + 0.25f) * 10));
        }
    }
    else
    {
        if (!_isFirstJumpCompleted)
        {
            _isFirstJumpCompleted = true;
        }
    }
    
    vertForce = clamp(vertForce, _body->GetMass() * 8.0f, 0);
    _body->ApplyLinearImpulse(b2Vec2(sideForce,vertForce), _body->GetWorldCenter(), true);
    
    _isMainAction = inputState->isMainAction();
    
    if (isPending)
    {
        playNetworkBoundSounds(numJumpsLastKnown, isSprintingLastKnown);
    }
}

void Robot::setAddressHash(uint64_t addressHash)
{
    _addressHash = addressHash;
}

uint64_t Robot::getAddressHash() const
{
    return _addressHash;
}

void Robot::setPlayerId(uint8_t inPlayerId)
{
    _playerId = inPlayerId;
}

uint8_t Robot::getPlayerId() const
{
    return _playerId;
}

void Robot::setPlayerName(std::string playerName)
{
    _playerName = playerName;
}

std::string& Robot::getPlayerName()
{
    return _playerName;
}

uint8_t Robot::getHealth()
{
    return _health;
}

uint8_t Robot::getNumJumps()
{
    return _numJumps;
}

bool Robot::isFacingLeft()
{
    return _isFacingLeft;
}

bool Robot::isMainAction()
{
    return _isMainAction;
}

bool Robot::isSprinting()
{
    return _isSprinting;
}

void Robot::playNetworkBoundSounds(uint8_t numJumpsLastKnown, bool isSprintingLastKnown)
{
    if (_numJumps > numJumpsLastKnown)
    {
        Util::playSound(SOUND_ID_ROBOT_JUMP, getPosition(), _isServer);
    }
    
    if (!isSprintingLastKnown && _isSprinting)
    {
        Util::playSound(SOUND_ID_ACTIVATE_SPRINT, getPosition(), _isServer);
    }
}
