#include <RoboCatShared.h>
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
	return DoRunLoop();
}

void Engine::HandleEvent( int inEvent )
{
	// Default implementation does nothing, up to derived classes to handle them, if they so choose
	( void )inEvent;
}

int Engine::DoRunLoop()
{
	// Main message loop
//	bool quit = false;
//	SDL_Event event;
//	memset( &event, 0, sizeof( SDL_Event ) );
//
//	while( !quit && mShouldKeepRunning )
//	{
//		if( SDL_PollEvent( &event ) )
//		{
//			if( event.type == SDL_QUIT )
//			{
//				quit = true;
//			}
//			else
//			{
//				HandleEvent( &event );
//			}
//		}
//		else
//		{
//			Timing::sInstance.Update();
//
//			DoFrame();
//		}
//	}

    return -1;//event.type;
}

void Engine::DoFrame()
{
	World::sInstance->Update();
}
