//
//  World.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include "GameObject.h"

#include <memory>
#include <vector>

class World
{
public:
	static void StaticInit();

	static std::unique_ptr<World> sInstance;

	void AddGameObject(GameObjectPtr inGameObject);
	void RemoveGameObject(GameObjectPtr inGameObject);

	void Update();

	const std::vector<GameObjectPtr >&	GetGameObjects()	const	{ return mGameObjects; }

private:
	World();

	int	GetIndexOfGameObject(GameObjectPtr inGameObject);

	std::vector<GameObjectPtr > mGameObjects;
};

#endif /* defined(__noctisgames__World__) */
