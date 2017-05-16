// server.cpp : Defines the entry point for the console application.
//

#include "pch.h"

#include "Server.h"

int main()
{
	if (Server::StaticInit())
	{
		return Server::sInstance->Run();
	}

	return -1;
}

