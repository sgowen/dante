#include "RoboCatShared.h"
#include <time.h>

std::unique_ptr< Engine >	Engine::sInstance;

Engine::Engine() :
mShouldKeepRunning( true )
{
	SocketUtil::StaticInit();

	srand( static_cast< uint32_t >( time( nullptr ) ) );
	
	GameObjectRegistry::StaticInit();

	World::StaticInit();

	ScoreBoardManager::StaticInit();
}

Engine::~Engine()
{
	SocketUtil::CleanUp();
}

int Engine::Run()
{
    // Override this
    return -2;
}

void Engine::DoFrame()
{
    Timing::sInstance.Update();
    
	World::sInstance->Update();
}
