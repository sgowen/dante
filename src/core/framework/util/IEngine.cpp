//
//  IEngine.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IEngine.h"

#include "FrameworkConstants.h"

IEngine::IEngine() : m_iRequestedAction(REQUESTED_ACTION_UPDATE)
{
    // Empty
}

IEngine::~IEngine()
{
    // Empty
}

int IEngine::getRequestedAction()
{
    return m_iRequestedAction;
}

void IEngine::clearRequestedAction()
{
    m_iRequestedAction = REQUESTED_ACTION_UPDATE;
}

RTTI_IMPL_NOPARENT(IEngine);
