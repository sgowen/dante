//
//  GameObjectRegistry.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameObjectRegistry__
#define __noctisgames__GameObjectRegistry__

#include "GameObject.h"

#include "NetworkConstants.h"

#include <unordered_map>

typedef GameObjectPtr (*GameObjectCreationFunc)();

class GameObjectRegistry
{
public:
	static void StaticInit();

	static std::unique_ptr<GameObjectRegistry> sInstance;

	void RegisterCreationFunction(uint32_t inFourCCName, GameObjectCreationFunc inCreationFunction);

	GameObjectPtr CreateGameObject(uint32_t inFourCCName);

private:
	GameObjectRegistry();

    std::unordered_map<uint32_t, GameObjectCreationFunc>	mNameToGameObjectCreationFunctionMap;
};

#endif /* defined(__noctisgames__GameObjectRegistry__) */
