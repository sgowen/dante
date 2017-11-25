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
#include "game/network/MainInputState.h"
#include "framework/network/portable/Move.h"
#include "game/logic/Ground.h"
#include "game/logic/Crate.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/math/NGRect.h"
#include "game/logic/Projectile.h"
#include "framework/entity/EntityRegistry.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/InputManager.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/network/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/Util.h"
#include "game/network/Server.h"
#include "game/logic/Ground.h"
#include "game/logic/SpacePirate.h"
#include "game/logic/SpacePirateChunk.h"

#include <math.h>

NGRTTI_IMPL(Robot, Entity);

NW_TYPE_IMPL(Robot);

Robot::Robot(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f, 2.0f, isServer, constructEntityDef()),
_addressHash(0),
_firstProjectileId(0),
_playerId(0),
_playerName("Robot"),
_numJumps(0),
_nextProjectileIndex(0),
_health(25),
_numKills(0),
_wasLastKillHeadshot(false),
_isFacingLeft(false),
_isShooting(false),
_isSprinting(false),
_isFirstJumpCompleted(false),
_speed(7.5f),
_jumpSpeed(11.0f),
_shotCooldownTime(0.0f),
_numSpacePiratesTouching(0),
_hasInitializedProjectiles(false),
_numJumpsLastKnown(0),
_nextProjectileIndexLastKnown(0),
_healthLastKnown(25),
_numKillsLastKnown(0),
_shotCooldownTimeLastKnown(0),
_wasLastKillHeadshotLastKnown(false),
_isFacingLeftLastKnown(false),
_isShootingLastKnown(false),
_isSprintingLastKnown(false),
_isFirstJumpCompletedLastKnown(false),
_isPending(false)
{
    for (uint8_t i = 0; i < NUM_PROJECTILES; ++i)
    {
        _projectiles[i] = NULL;
        
        if (_isServer)
        {
            _projectiles[i] = static_cast<Projectile*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_Projectile));
            
            if (i == 0)
            {
                _firstProjectileId = _projectiles[i]->getID();
            }
        }
    }
}

Robot::~Robot()
{
    if (_isServer)
    {
        for (Projectile* p : _projectiles)
        {
            p->requestDeletion();
        }
    }
}

EntityDef Robot::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    ret.isCharacter = true;
    ret.density = 16.0f;
    
    return ret;
}

void Robot::update()
{
    _stateTime += FRAME_RATE;
    
    _shotCooldownTime -= FRAME_RATE;
    
    if (_isShooting)
    {
        if (_shotCooldownTime < 0)
        {
            _shotCooldownTime = FRAME_RATE * (60.0f / NUM_PROJECTILES);
            
            fireProjectile();
        }
    }
    
    if (getVelocity().y < 0 && !isFalling() && _numJumps > 0)
    {
        _stateTime = 0;
    }
    
    if (_isServer)
    {
        if (_numSpacePiratesTouching > 0 && _health > 0)
        {
            --_health;
        }
        
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
        
        if (_healthLastKnown != _health
            || _numKillsLastKnown != _numKills
            || _wasLastKillHeadshotLastKnown != _wasLastKillHeadshot)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Stats);
        }
    }
    else
    {
        if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
        {
            if (_numKills > _numKillsLastKnown && _wasLastKillHeadshot)
            {
                Util::playSound(SOUND_ID_HEADSHOT, getPosition(), _isServer);
            }
            
            if ((_readState & ROBT_PlayerInfo) != 0)
            {
                NG_AUDIO_ENGINE->playSound(SOUND_ID_DEATH);
            }
        }
        else
        {
            playNetworkBoundSounds(_numJumpsLastKnown, _isSprintingLastKnown);
        }
        
        if (!_hasInitializedProjectiles)
        {
            bool isInitialized = true;
            for (uint8_t i = 0; i < NUM_PROJECTILES; ++i)
            {
                _projectiles[i] = static_cast<Projectile*>(FWInstanceManager::getClientEntityManager()->getEntityByID(_firstProjectileId + i));
                
                if (!_projectiles[i])
                {
                    isInitialized = false;
                    break;
                }
            }
            
            _hasInitializedProjectiles = isInitialized;
        }
    }
    
    _velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    _positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    _numJumpsLastKnown = _numJumps;
    _nextProjectileIndexLastKnown = _nextProjectileIndex;
    _healthLastKnown = _health;
    _numKillsLastKnown = _numKills;
    _shotCooldownTimeLastKnown = _shotCooldownTime;
    _wasLastKillHeadshotLastKnown = _wasLastKillHeadshot;
    _isFacingLeftLastKnown = _isFacingLeft;
    _isShootingLastKnown = _isShooting;
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
    
    return inEntity->getRTTI().derivesFrom(SpacePirate::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti)
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
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        ++_numSpacePiratesTouching;
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
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        --_numSpacePiratesTouching;
    }
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    _readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_addressHash);
        inInputStream.read(_firstProjectileId);
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
        inInputStream.read<uint8_t, 3>(_nextProjectileIndex);
        inInputStream.read<uint8_t, 1>(_numGroundContacts);
        inInputStream.read(_shotCooldownTime);
        
        inInputStream.read(_isFacingLeft);
        inInputStream.read(_isShooting);
        inInputStream.read(_isSprinting);
        inInputStream.read(_isFirstJumpCompleted);
        
        _readState |= ROBT_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_health);
        
        inInputStream.read(_numKills);
        inInputStream.read(_wasLastKillHeadshot);
        
        _readState |= ROBT_Stats;
    }
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ROBT_PlayerInfo)
    {
        inOutputStream.write(true);
        inOutputStream.write(_addressHash);
        inOutputStream.write(_firstProjectileId);
        inOutputStream.write(_playerId);
        inOutputStream.write(_playerName);
        inOutputStream.write(_color);
        
        writtenState |= ROBT_PlayerInfo;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & ROBT_Pose)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_stateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write<uint8_t, 2>(_numJumps);
        inOutputStream.write<uint8_t, 3>(_nextProjectileIndex);
        inOutputStream.write<uint8_t, 1>(_numGroundContacts);
        inOutputStream.write(_shotCooldownTime);
        
        inOutputStream.write(_isFacingLeft);
        inOutputStream.write(_isShooting);
        inOutputStream.write(_isSprinting);
        inOutputStream.write(_isFirstJumpCompleted);
        
        writtenState |= ROBT_Pose;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & ROBT_Stats)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_health);
        
        inOutputStream.write(_numKills);
        inOutputStream.write(_wasLastKillHeadshot);
        
        writtenState |= ROBT_Stats;
    }
    else
    {
        inOutputStream.write(false);
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
    
    _isPending = isPending;
    
    uint8_t numJumpsLastKnown = _numJumps;
    bool isSprintingLastKnown = _isSprinting;
    
    b2Vec2 velocity = b2Vec2(getVelocity().x, getVelocity().y);
    
    velocity.Set(inputState->getDesiredRightAmount() * _speed, getVelocity().y);
    
    _isFacingLeft = velocity.x < 0 ? true : velocity.x > 0 ? false : _isFacingLeft;
    
    _isSprinting = inputState->isSprinting();
    
    if (_isSprinting)
    {
        velocity += b2Vec2(inputState->getDesiredRightAmount() * _speed / 2, 0);
    }
    
    if (inputState->isJumping())
    {
        if (isGrounded() && _numJumps == 0)
        {
            _stateTime = 0;
            _isFirstJumpCompleted = false;
            _numJumps = 1;
            
            velocity.Set(velocity.x, _jumpSpeed);
        }
        else if (_numJumps == 1 && _isFirstJumpCompleted)
        {
            _stateTime = 0;
            _numJumps = 2;
            
            velocity.Set(velocity.x, _jumpSpeed - 3);
        }
    }
    else
    {
        if (_numJumps == 1)
        {
            _isFirstJumpCompleted = true;
            
            if (velocity.y > (_jumpSpeed / 2.0f))
            {
                velocity.Set(velocity.x, _jumpSpeed / 2.0f);
            }
        }
        else if (_numJumps == 2 && velocity.y > ((_jumpSpeed - 2) / 2.0f))
        {
            velocity.Set(velocity.x, (_jumpSpeed - 2) / 2.0f);
        }
    }
    
    _isShooting = inputState->isShooting();
    
    setVelocity(velocity);
    
    if (isPending)
    {
        playNetworkBoundSounds(numJumpsLastKnown, isSprintingLastKnown);
    }
}

void Robot::awardKill(bool isHeadshot)
{
    if (!_isServer)
    {
        return;
    }
    
    _numKills++;
    
    _wasLastKillHeadshot = isHeadshot;
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
    
    for (uint8_t i = 0; i < NUM_PROJECTILES; ++i)
    {
        if (_projectiles[i])
        {
            _projectiles[i]->initFromShooter(this);
        }
    }
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

uint32_t Robot::getNumKills()
{
    return _numKills;
}

uint8_t Robot::getHealth()
{
    return _health;
}

bool Robot::isFacingLeft()
{
    return _isFacingLeft;
}

bool Robot::isShooting()
{
    return _isShooting;
}

bool Robot::isSprinting()
{
    return _isSprinting;
}

bool Robot::isPending()
{
    return _isPending;
}

void Robot::fireProjectile()
{
    Projectile* projectile = _projectiles[_nextProjectileIndex++];
    
    if (_nextProjectileIndex >= NUM_PROJECTILES)
    {
        _nextProjectileIndex = 0;
    }
    
    if (projectile)
    {
        projectile->fire(this);
        
        if (_isPending)
        {
            // This projectile was just created
            Util::playSound(SOUND_ID_FIRE_ROCKET, getPosition(), _isServer);
        }
    }
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
