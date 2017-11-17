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

ReplicationCommand::ReplicationCommand(uint32_t inInitialDirtyState) : _action(RA_Create), _dirtyState(inInitialDirtyState)
{
    // Empty
}

void ReplicationCommand::handleCreateAckd()
{
    if (_action == RA_Create)
    {
        // if the create is ack'd, we can demote to just an update...
        _action = RA_Update;
    }
}

void ReplicationCommand::addDirtyState(uint32_t inState)
{
    _dirtyState |= inState;
}

void ReplicationCommand::setDestroy()
{
    _action = RA_Destroy;
}

bool ReplicationCommand::hasDirtyState() const
{
    return (_action == RA_Destroy) || (_dirtyState != 0);
}

ReplicationAction ReplicationCommand::getAction() const
{
    return _action;
}

uint32_t ReplicationCommand::getDirtyState() const
{
    return _dirtyState;
}

void ReplicationCommand::clearDirtyState(uint32_t inStateToClear)
{
    _dirtyState &= ~inStateToClear;
    
    if (_action == RA_Destroy)
    {
        _action = RA_Update;
    }
}
