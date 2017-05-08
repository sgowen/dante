
#include <RoboCatClientPCH.h>

bool Client::StaticInit( )
{
	// Create the Client pointer first because it initializes SDL
	Client* client = new Client();

	RenderManager::StaticInit();
	InputManager::StaticInit();

	HUD::StaticInit();

	sInstance.reset( client );

	return true;
}

Client::Client()
{
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'RCAT', RoboCatClient::StaticCreate );
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'MOUS', MouseClient::StaticCreate );
	GameObjectRegistry::sInstance->RegisterCreationFunction( 'YARN', YarnClient::StaticCreate );

	string destination = StringUtils::GetCommandLineArg( 1 );
	string name = StringUtils::GetCommandLineArg( 2 );

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("localhost:9999");

	NetworkManagerClient::StaticInit( *serverAddress, "Stephen" );

	//NetworkManagerClient::sInstance->SetDropPacketChance( 0.6f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.25f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.5f );
	//NetworkManagerClient::sInstance->SetSimulatedLatency( 0.1f );
}



void Client::DoFrame()
{
	InputManager::sInstance->Update();

	Engine::DoFrame();

	NetworkManagerClient::sInstance->ProcessIncomingPackets();

	RenderManager::sInstance->Render();

	NetworkManagerClient::sInstance->SendOutgoingPackets();
}

void Client::HandleEvent( int inEvent )
{
	switch (inEvent)
	{
	case 0:
		InputManager::sInstance->HandleInput( EIA_Pressed, 'D');
		break;
	case 1:
		InputManager::sInstance->HandleInput( EIA_Released, 'W');
		break;
	default:
		break;
	}
}

