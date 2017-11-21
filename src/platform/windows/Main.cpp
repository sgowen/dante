//
// Main.cpp
//

#include "pch.h"

#include "framework/main/directx/DirectXMain.h"

#include "game/logic/MainEngine.h"

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	return DirectXMain::exec(hInstance, hPrevInstance, lpCmdLine, nCmdShow, new MainEngine());
}
