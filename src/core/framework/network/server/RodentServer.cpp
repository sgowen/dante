#include "RoboCatServerPCH.h"


RodentServer::RodentServer()
{
}

void RodentServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject( this );
}


bool RodentServer::HandleCollisionWithCat( RoboCat* inCat )
{
	//kill yourself!
	SetDoesWantToDie( true );

	ScoreBoardManager::sInstance->IncScore( inCat->GetPlayerId(), 1 );

	return false;
}


