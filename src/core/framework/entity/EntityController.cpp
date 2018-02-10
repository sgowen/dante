//
//  EntityController.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityController.h>

NGRTTI_IMPL_NOPARENT(EntityController);

EntityController::EntityController(Entity* inEntity) : _entity(inEntity)
{
    // Empty
}

EntityController::~EntityController()
{
    // Empty
}
