//
//  InputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatClientPCH.h"

unique_ptr< InputManager >	InputManager::sInstance;

namespace
{
	float kTimeBetweenInputSamples = 0.03f;
}

void InputManager::StaticInit()
{
	sInstance.reset(new InputManager());
}

namespace
{
    inline void UpdateDesireFloatFromKey(EInputAction inInputAction, float& ioVariable)
	{
		if (inInputAction == EIA_Pressed)
		{
			ioVariable = 1.f;
		}
		else if (inInputAction == EIA_Released)
		{
			ioVariable = 0.f;
		}
	}
}

void InputManager::HandleInput(EInputAction inInputAction, char inKeyCode)
{
	switch(inKeyCode)
	{
	case 'D':
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredLeftAmount);
		break;
	case 'A':
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredRightAmount);
		break;
	case 'W':
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredForwardAmount);
		break;
	case 'S':
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredBackAmount);
		break;
	}
}

InputManager::InputManager() :
	mNextTimeToSampleInput(0.f),
	mPendingMove(nullptr)
{

}

const Move& InputManager::SampleInputAsMove()
{
	return mMoveList.AddMove(GetState(), Timing::sInstance.GetFrameStartTime());
}

bool InputManager::IsTimeToSampleInput()
{
	float time = Timing::sInstance.GetFrameStartTime();
	if (time > mNextTimeToSampleInput)
	{
		mNextTimeToSampleInput = mNextTimeToSampleInput + kTimeBetweenInputSamples;
		return true;
	}

	return false;
}

void InputManager::Update()
{
	if (IsTimeToSampleInput())
	{
		mPendingMove = &SampleInputAsMove();
	}
}
