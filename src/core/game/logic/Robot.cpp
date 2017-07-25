//
//  Robot.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Robot.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "InputState.h"
#include "Move.h"
#include "Ground.h"
#include "Crate.h"

#include "World.h"
#include "macros.h"
#include "GameConstants.h"
#include "Timing.h"
#include "StringUtil.h"
#include "MathUtil.h"
#include "NGRect.h"
#include "Projectile.h"
#include "EntityRegistry.h"
#include "NetworkManagerServer.h"
#include "ClientProxy.h"
#include "NetworkManagerClient.h"
#include "InputManager.h"
#include "NGAudioEngine.h"
#include "InstanceManager.h"
#include "FWInstanceManager.h"
#include "Util.h"
#include "Server.h"
#include "Ground.h"
#include "SpacePirate.h"

#include <math.h>

Robot::Robot(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f, 2.0f, constructEntityDef()),
m_iAddressHash(0),
m_iPlayerId(0),
m_playerName("Robot"),
m_iNumJumps(0),
m_isFacingLeft(false),
m_isShooting(false),
m_isSprinting(false),
m_iHealth(25),
m_iNumKills(0),
m_wasLastKillHeadshot(false),
m_iReadState(0),
m_fSpeed(7.5f),
m_fJumpSpeed(11.0f),
m_fTimeOfNextShot(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_isServer(isServer),
m_isGrounded(false),
m_isFalling(false),
m_isFirstJumpCompleted(false),
m_velocityOld(),
m_positionOld(),
m_iNumJumpsOld(0),
m_isSprintingOld(false),
m_isHost(false),
m_hostBody(nullptr),
m_hostFixture(nullptr)
{
    if (m_isServer)
    {
        b2World& hostWorld = InstanceManager::getServerWorld()->getWorld(0);
        m_isHost = &hostWorld == &world;
        if (!m_isHost)
        {
            // Define the dynamic body. We set its position and call the body factory.
            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position.Set(getPosition().x, getPosition().y);
            bodyDef.fixedRotation = m_body->IsFixedRotation();
            bodyDef.bullet = m_body->IsBullet();
            m_hostBody = hostWorld.CreateBody(&bodyDef);
            
            // Define another box shape for our dynamic body.
            b2PolygonShape dynamicBox;
            dynamicBox.SetAsBox(m_fWidth / 2.0f, m_fHeight / 2.0f);
            
            // Define the dynamic body fixture.
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &dynamicBox;
            fixtureDef.isSensor = m_fixture->IsSensor();
            
            // Set the box density to be non-zero, so it will be dynamic.
            fixtureDef.density = m_fixture->GetDensity();
            
            // Override the default friction.
            fixtureDef.friction = m_fixture->GetFriction();
            
            fixtureDef.restitution = m_fixture->GetRestitution();
            
            // Add the shape to the body.
            m_hostFixture = m_hostBody->CreateFixture(&fixtureDef);
            
            m_hostFixture->SetUserData(this);
            
            m_hostBody->SetUserData(this);
        }
    }
}

Robot::~Robot()
{
    if (m_hostFixture)
    {
        m_hostBody->DestroyFixture(m_hostFixture);
        m_hostFixture = nullptr;
    }
    
    if (m_hostBody)
    {
        b2World& hostWorld = InstanceManager::getServerWorld()->getWorld(0);
        hostWorld.DestroyBody(m_hostBody);
        m_hostBody = nullptr;
    }
}

EntityDef Robot::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    
    return ret;
}

void Robot::update()
{
    if (m_isServer)
    {
        b2Vec2 oldVelocity = b2Vec2(getVelocity().x, getVelocity().y);
        b2Vec2 oldPosition = b2Vec2(getPosition().x, getPosition().y);
        uint8_t oldNumJumps = m_iNumJumps;
        bool old_m_isGrounded = m_isGrounded;
        bool old_m_isFalling = m_isFalling;
        bool old_isFacingLeft = m_isFacingLeft;
        bool old_isShooting = m_isShooting;
        bool old_isSprinting = m_isSprinting;
        
        // is there a move we haven't processed yet?
        ClientProxy* client = NG_SERVER->getClientProxy(getPlayerId());
        if (client)
        {
            MoveList& moveList = client->getUnprocessedMoveList();
            for (const Move& unprocessedMove : moveList)
            {
                processMove(unprocessedMove);
            }
        }
        
        handleShooting();
        
        if (!areBox2DVectorsEqual(oldVelocity, getVelocity())
            || !areBox2DVectorsEqual(oldPosition, getPosition())
            || oldNumJumps != m_iNumJumps
            || old_m_isGrounded != m_isGrounded
            || old_m_isFalling != m_isFalling
            || old_isFacingLeft != m_isFacingLeft
            || old_isShooting != m_isShooting
            || old_isSprinting != m_isSprinting)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Pose);
        }
    }
    else
    {
        const Move* pendingMove = InputManager::getInstance()->getPendingMove();
        if (pendingMove)
        {
            if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
            {
                processMove(*pendingMove);
            }
            else
            {
                updateInternal(pendingMove->getDeltaTime());
            }
        }
    }
}

bool Robot::shouldCollide(Entity *inEntity)
{
    return inEntity->getRTTI().derivesFrom(SpacePirate::rtti) || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void Robot::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            (static_cast<SpacePirate*>(inEntity))->handleContactWithRobot(this);
        }
        else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
        {
            handleContactWithGround(nullptr);
        }
        else if (inEntity->getRTTI().derivesFrom(Crate::rtti))
        {
            handleContactWithCrate(nullptr);
        }
    }
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    m_velocityOld = b2Vec2(getVelocity().x, getVelocity().y);
    m_positionOld = b2Vec2(getPosition().x, getPosition().y);
    m_iNumJumpsOld = m_iNumJumps;
    m_isSprintingOld = m_isSprinting;
    
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iAddressHash);
        inInputStream.read(m_iPlayerId);
        inInputStream.read(m_playerName);
        inInputStream.read(m_color);
        
        m_iReadState |= ROBT_PlayerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        inInputStream.read(m_iNumJumps);
        
        inInputStream.read(m_iHealth);
        
        inInputStream.read(m_iNumKills);
        inInputStream.read(m_wasLastKillHeadshot);
        
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isShooting);
        inInputStream.read(m_isSprinting);
        
        m_iReadState |= ROBT_Pose;
    }
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ROBT_PlayerInfo)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iAddressHash);
        inOutputStream.write(m_iPlayerId);
        inOutputStream.write(m_playerName);
        inOutputStream.write(m_color);
        
        writtenState |= ROBT_PlayerInfo;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(m_iNumJumps);
        
        inOutputStream.write(m_iHealth);
        
        inOutputStream.write(m_iNumKills);
        inOutputStream.write((bool)m_wasLastKillHeadshot);
        
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isShooting);
        inOutputStream.write((bool)m_isSprinting);
        
        writtenState |= ROBT_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Robot::postRead()
{
    if (m_iReadState > 0)
    {
        if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
        {
            doClientSidePredictionForLocalRobot();
            
            // if this is a create packet, don't interpolate
            if ((m_iReadState & ROBT_PlayerInfo) == 0)
            {
                interpolateClientSidePrediction(m_velocityOld, m_positionOld);
            }
        }
        else
        {
            doClientSidePredictionForRemoteRobot();
            
            playNetworkBoundSounds(m_iNumJumpsOld, m_isSprintingOld);
        }
        
        m_iReadState = 0;
    }
}

void Robot::syncToHost()
{
    if (!m_isHost && m_hostBody)
    {
        m_hostBody->SetTransform(b2Vec2(getPosition().x, getPosition().y), m_body->GetAngle());
        m_hostBody->SetLinearVelocity(b2Vec2(getPosition().x, getPosition().y));
    }
}

void Robot::handleContactWithGround(Ground* ground)
{
    m_iNumJumps = 0;
    m_isGrounded = true;
    m_isFalling = false;
    
    if (m_isServer)
    {
        NG_SERVER->setStateDirty(getID(), ROBT_Pose);
    }
}

void Robot::handleContactWithCrate(Crate* inCrate)
{
    handleContactWithGround(nullptr);
}

void Robot::takeDamage()
{
    if (m_iHealth > 0)
    {
        m_iHealth--;
    }
    
    if (m_isServer)
    {
        // tell the world our health dropped
        NG_SERVER->setStateDirty(getID(), ROBT_Pose);
    }
}

void Robot::awardKill(bool isHeadshot)
{
    m_iNumKills++;
    
    m_wasLastKillHeadshot = isHeadshot;
    
    if (m_isServer)
    {
        // tell the world how bad ass we are
        NG_SERVER->setStateDirty(getID(), ROBT_Pose);
    }
    else
    {
        playSound(SOUND_ID_HEADSHOT);
    }
}

void Robot::setAddressHash(uint64_t addressHash)
{
    m_iAddressHash = addressHash;
}

uint64_t Robot::getAddressHash() const
{
    return m_iAddressHash;
}

void Robot::setPlayerId(uint8_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint8_t Robot::getPlayerId() const
{
    return m_iPlayerId;
}

void Robot::setPlayerName(std::string playerName)
{
    m_playerName = playerName;
}

std::string& Robot::getPlayerName()
{
    return m_playerName;
}

uint32_t Robot::getNumKills()
{
    return m_iNumKills;
}

uint8_t Robot::getHealth()
{
    return m_iHealth;
}

bool Robot::isFacingLeft()
{
    return m_isFacingLeft;
}

bool Robot::isGrounded()
{
    return m_isGrounded;
}

bool Robot::isShooting()
{
    return m_isShooting;
}

bool Robot::isSprinting()
{
    return m_isSprinting;
}

void Robot::processMove(const Move& inMove)
{
    uint8_t old_m_iNumJumps = m_iNumJumps;
    bool old_m_isSprinting = m_isSprinting;
    
    processInput(inMove.getInputState());
    
    updateInternal(inMove.getDeltaTime());
    
    playNetworkBoundSounds(old_m_iNumJumps, old_m_isSprinting);
}

void Robot::processInput(IInputState* inInputState)
{
    InputState* is = static_cast<InputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    InputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == nullptr)
    {
        return;
    }
    
    b2Vec2 velocity = b2Vec2(getVelocity().x, getVelocity().y);
    
    if (m_isGrounded)
    {
        velocity.Set(inputState->getDesiredRightAmount() * m_fSpeed, getVelocity().y);
        
        m_isFacingLeft = velocity.x < 0 ? true : velocity.x > 0 ? false : m_isFacingLeft;
        
        m_isSprinting = inputState->isSprinting();
        
        if (m_isSprinting)
        {
            velocity += b2Vec2(inputState->getDesiredRightAmount() * m_fSpeed / 2, 0);
        }
    }
    else
    {
        velocity.Set(inputState->getDesiredRightAmount() * m_fSpeed / 2, getVelocity().y);
        
        m_isFacingLeft = velocity.x < 0 ? true : velocity.x > 0 ? false : m_isFacingLeft;
    }
    
    if (inputState->isJumping())
    {
        if (m_isGrounded && areFloatsPracticallyEqual(getAngle(), 0))
        {
            m_fStateTime = 0;
            m_isGrounded = false;
            m_isFalling = false;
            m_isFirstJumpCompleted = false;
            m_iNumJumps = 1;
            
            velocity.Set(velocity.x, m_fJumpSpeed);
        }
        else if (m_iNumJumps == 1 && m_isFirstJumpCompleted)
        {
            m_fStateTime = 0;
            m_iNumJumps = 2;
            
            velocity.Set(velocity.x, m_fJumpSpeed - 3);
        }
    }
    else
    {
        if (m_iNumJumps == 1)
        {
            m_isFirstJumpCompleted = true;
            
            if (velocity.y > (m_fJumpSpeed / 2.0f))
            {
                velocity.Set(velocity.x, m_fJumpSpeed / 2.0f);
            }
        }
        
        if (m_iNumJumps == 2 && velocity.y > ((m_fJumpSpeed - 2) / 2.0f))
        {
            velocity.Set(velocity.x, (m_fJumpSpeed - 2) / 2.0f);
        }
    }
    
    m_isShooting = inputState->isShooting();
    
    setVelocity(velocity);
}

void Robot::updateInternal(float inDeltaTime)
{
    m_fStateTime += inDeltaTime;
    
    stepPhysics(inDeltaTime);
    
    if (m_isGrounded)
    {
        setAngle(0);
    }
    
    if (getVelocity().y < 0 && !m_isFalling && m_iNumJumps > 0)
    {
        m_isFalling = true;
        m_fStateTime = 0;
    }
    
    if (getPosition().y < DEAD_ZONE_BOTTOM)
    {
        m_iHealth = 0;
    }
    
    if (m_iHealth == 0 && !isRequestingDeletion())
    {
        playSound(SOUND_ID_DEATH);
        
        // TODO, this is NOT the right way to handle the player dying
        
        requestDeletion();
        
        if (m_isServer)
        {
            Server::sHandleNewClient(m_iPlayerId, m_playerName);
        }
    }
}
void Robot::stepPhysics(float deltaTime)
{
    if (!m_isServer)
    {
        if (!NG_CLIENT->isPlayerIdLocal(getPlayerId())
            || !InputManager::getInstance()->isPlayerIdLocalHost(getPlayerId()))
        {
            return;
        }
    }
    
    static int32 velocityIterations = 12;
    static int32 positionIterations = 4;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    m_worldRef.Step(deltaTime, velocityIterations, positionIterations);
}

void Robot::handleShooting()
{
    if (!m_isServer)
    {
        return;
    }
    
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (m_isShooting && time > m_fTimeOfNextShot)
    {
        // not exact, but okay
        m_fTimeOfNextShot = time + 0.05f;
        
        // fire!
        Projectile* projectile = static_cast<Projectile*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_Projectile));
        projectile->initFromShooter(this);
    }
}

void Robot::doClientSidePredictionForLocalRobot()
{
    if (!NG_CLIENT->isPlayerIdLocal(getPlayerId())
        || !InputManager::getInstance()->isPlayerIdLocalHost(getPlayerId()))
    {
        return;
    }
    
    if ((m_iReadState & ROBT_Pose) != 0)
    {
        // simulate pose only if we received new pose
        // all processed moves have been removed, so all that are left are unprocessed moves
        // so we must apply them...
        MoveList& moveList = InputManager::getInstance()->getMoveList();
        
        for (const Move& move : moveList)
        {
            for (Entity* entity : InstanceManager::getClientWorld()->getPlayers())
            {
                Robot* robot = static_cast<Robot*>(entity);
                robot->processInput(move.getInputState());
            }
            
            for (Entity* entity : InstanceManager::getClientWorld()->getPlayers())
            {
                Robot* robot = static_cast<Robot*>(entity);
                robot->updateInternal(move.getDeltaTime());
            }
        }
    }
}

void Robot::doClientSidePredictionForRemoteRobot()
{
    // so what do we want to do here? need to do some kind of interpolation...
    
    if ((m_iReadState & ROBT_Pose) != 0)
    {
        // simulate movement for an additional RTT/2
        float rtt = NG_CLIENT->getRoundTripTime() / 2;
        
        // break into framerate sized chunks so we don't run through walls and do crazy things...
        while (true)
        {
            if (rtt < FRAME_RATE)
            {
                updateInternal(rtt);
                break;
            }
            else
            {
                updateInternal(FRAME_RATE);
                rtt -= FRAME_RATE;
            }
        }
    }
}

void Robot::interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), m_fTimeVelocityBecameOutOfSync, "velocity"))
    {
        setVelocity(inOldVelocity);
    }
    
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync, "position"))
    {
        setPosition(inOldPos);
    }
}

bool Robot::interpolateVectorsIfNecessary(b2Vec2& inOld, const b2Vec2& inNew, float& syncTracker, const char* vectorType)
{
    float rtt = NG_CLIENT->getRoundTripTime();
    rtt /= 2; // Let's just measure the time from server to us
    
    if (!areBox2DVectorsEqual(inOld, inNew))
    {
        LOG("WARN: Robot move replay ended with incorrect %s! Old %3.8f, %3.8f - New %3.8f, %3.8f", vectorType, inOld.x, inOld.y, inNew.x, inNew.y);
        
        // have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (syncTracker == 0.0f)
        {
            syncTracker = time;
        }
        
        // now interpolate to the correct value...
        float durationOutOfSync = time - syncTracker;
        if (durationOutOfSync < rtt)
        {
            b2Vec2 vec = lerpBox2DVector(inOld, inNew, rtt);
            inOld.Set(vec.x, vec.y);
            
            return true;
        }
    }
    else
    {
        syncTracker = 0.0f;
    }
    
    return false;
}

void Robot::playNetworkBoundSounds(uint8_t old_m_iNumJumps, bool old_m_isSprinting)
{
    if (old_m_iNumJumps != m_iNumJumps && m_iNumJumps > 0)
    {
        playSound(SOUND_ID_ROBOT_JUMP);
    }
    
    if (!old_m_isSprinting && m_isSprinting)
    {
        playSound(SOUND_ID_ACTIVATE_SPRINT);
    }
}

void Robot::playSound(int soundId)
{
    Util::playSound(soundId, getPosition(), m_isServer);
}

RTTI_IMPL(Robot, Entity);

NW_TYPE_IMPL(Robot);
