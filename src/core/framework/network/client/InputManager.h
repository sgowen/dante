//
//  InputManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputManager__
#define __noctisgames__InputManager__

#include "InputAction.h"
#include "InputState.h"
#include "MoveList.h"

#include <memory>

class Move;

class InputManager
{
public:
	static void StaticInit();
    static std::unique_ptr<InputManager >	sInstance;

	void HandleInput(EInputAction inInputAction, char inKeyCode);

	const InputState& GetState()	const	{ return mCurrentState; }

	MoveList& GetMoveList()		{ return mMoveList; }

	const Move* GetAndClearPendingMove()	{ auto toRet = mPendingMove; mPendingMove = nullptr; return toRet; }

	void Update();

private:

	InputState mCurrentState;

	InputManager();

	bool IsTimeToSampleInput();
	const Move& SampleInputAsMove();
	
	MoveList mMoveList;
	float mNextTimeToSampleInput;
	const Move* mPendingMove;
};

#endif /* defined(__noctisgames__InputManager__) */
