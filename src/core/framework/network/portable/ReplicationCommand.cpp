//
//  ReplicationCommand.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationCommand.h"

ReplicationCommand::ReplicationCommand()
{
    // Empty
}

ReplicationCommand::ReplicationCommand(uint32_t inInitialDirtyState) : m_action(RA_Create), m_iDirtyState(inInitialDirtyState)
{
    // Empty
}

void ReplicationCommand::handleCreateAckd()
{
    if (m_action == RA_Create)
    {
        // if the create is ack'd, we can demote to just an update...
        m_action = RA_Update;
    }
}

void ReplicationCommand::addDirtyState(uint32_t inState)
{
    m_iDirtyState |= inState;
}

void ReplicationCommand::setDestroy()
{
    m_action = RA_Destroy;
}

bool ReplicationCommand::hasDirtyState() const
{
    return (m_action == RA_Destroy) || (m_iDirtyState != 0);
}

ReplicationAction ReplicationCommand::getAction() const
{
    return m_action;
}

uint32_t ReplicationCommand::getDirtyState() const
{
    return m_iDirtyState;
}

void ReplicationCommand::clearDirtyState(uint32_t inStateToClear)
{
    m_iDirtyState &= ~inStateToClear;
    
    if (m_action == RA_Destroy)
    {
        m_action = RA_Update;
    }
}
