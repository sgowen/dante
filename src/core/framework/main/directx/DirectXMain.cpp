//
//  DirectXMain.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/main/directx/DirectXMain.h>

#include <framework/main/portable/EngineController.h>
#include <framework/main/portable/Engine.h>

#include <framework/graphics/directx/DirectXRendererHelper.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/KeyboardInputManager.h>
#include <framework/input/GamePadInputManager.h>
#include <framework/util/Constants.h>
#include <framework/util/macros.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/input/KeyboardLookup.h>
#include <framework/util/MathUtil.h>

#include "Audio.h"
#include "DirectXColors.h>
#include <Dbt.h>

extern void exitGame();

using namespace DirectX;

#define DX_MAIN (DirectXMain::getInstance())

DirectXMain* DirectXMain::s_instance = NULL;

void DirectXMain::create()
{
	assert(!s_instance);

	s_instance = new DirectXMain();
}

DirectXMain * DirectXMain::getInstance()
{
    return s_instance;
}

void DirectXMain::destroy()
{
	assert(s_instance);

	delete s_instance;
	s_instance = NULL;
}

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int DirectXMain::exec(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, EngineController* engineController)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!XMVerifyCPUSupport())
	{
		return 1;
	}

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
	{
		return 1;
	}
    
    if (!engineController)
    {
        return 1;
    }

	// Register class and create window
	{
		// Register class
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DirectXMain::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = L"NGWindowClass";
		wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassEx(&wcex))
		{
			return 1;
		}

		DirectXMain::create();

		// Create window
		int w, h;
		DX_MAIN->GetDefaultSize(w, h);

		RECT rc;
		rc.top = 0;
		rc.left = 0;
		rc.right = static_cast<LONG>(w);
		rc.bottom = static_cast<LONG>(h);

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

#ifndef _DEBUG
#define LAUNCH_FULL_SCREEN
#endif

#ifdef LAUNCH_FULL_SCREEN
		/// Launch directly into full screen
		HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, L"NGWindowClass", L"Project Dante", WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);
#else
		HWND hwnd = CreateWindowEx(0, L"NGWindowClass", L"Project Dante", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);
#endif

		if (!hwnd)
		{
			return 1;
		}

#ifdef LAUNCH_FULL_SCREEN
		/// Launch directly into full screen
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
#else
		ShowWindow(hwnd, nCmdShow);
#endif

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(DX_MAIN));

		GetClientRect(hwnd, &rc);

		DX_MAIN->Initialize(engineController, hwnd, rc.right - rc.left, rc.bottom - rc.top);

		HDEVNOTIFY hNewAudio = nullptr;

		// Listen for new audio devices
		DEV_BROADCAST_DEVICEINTERFACE filter = { 0 };
		filter.dbcc_size = sizeof(filter);
		filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		filter.dbcc_classguid = KSCATEGORY_AUDIO;

		hNewAudio = RegisterDeviceNotification(hwnd, &filter,
			DEVICE_NOTIFY_WINDOW_HANDLE);

		if (hNewAudio)
		{
			UnregisterDeviceNotification(hNewAudio);
		}
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DX_MAIN->Tick();
		}
	}

	DirectXMain::destroy();

	CoUninitialize();

	return (int)msg.wParam;
}

LRESULT CALLBACK DirectXMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;

#ifdef LAUNCH_FULL_SCREEN
	static bool s_fullscreen = true;
#else
	static bool s_fullscreen = false;
#endif

	auto directXMain = reinterpret_cast<DirectXMain*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_PAINT:
		if (s_in_sizemove && directXMain)
		{
			directXMain->Tick();
		}
		else
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && directXMain)
				{
					directXMain->OnSuspending();
				}
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && directXMain)
			{
				directXMain->OnResuming();
			}
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && directXMain)
		{
			directXMain->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (directXMain)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			directXMain->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

	case WM_GETMINMAXINFO:
	{
		auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;

	case WM_ACTIVATEAPP:
		if (directXMain)
		{
			if (wParam)
			{
				directXMain->OnActivated();
			}
			else
			{
				directXMain->OnDeactivated();
			}
		}

		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);

		break;

	case WM_POWERBROADCAST:
		switch (wParam)
		{
		case PBT_APMQUERYSUSPEND:
			if (!s_in_suspend && directXMain)
			{
				directXMain->OnSuspending();
			}
			s_in_suspend = true;
			return TRUE;

		case PBT_APMRESUMESUSPEND:
			if (!s_minimized)
			{
				if (s_in_suspend && directXMain)
				{
					directXMain->OnResuming();
				}
				s_in_suspend = false;
			}
			return TRUE;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// Implements the classic ALT+ENTER fullscreen toggle
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

				int width;
				int height;
				if (directXMain)
				{
					directXMain->GetDefaultSize(width, height);
				}

				ShowWindow(hWnd, SW_SHOWNORMAL);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}

		Keyboard::ProcessMessage(message, wParam, lParam);

		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_DEVICECHANGE:
		if (wParam == DBT_DEVICEARRIVAL)
		{
			auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
			if (pDev)
			{
				if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
				{
					auto pInter = reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
					if (pInter->dbcc_classguid == KSCATEGORY_AUDIO)
					{
						if (directXMain)
						{
							directXMain->OnNewAudioDevice();
						}
					}
				}
			}
		}
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Initialize the DirectX resources required to run.
void DirectXMain::Initialize(EngineController* engineController, HWND window, int width, int height)
{
	_deviceResources->SetWindow(window, width, height);
    
    _engine = new Engine(engineController);

	_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	_keyboard = std::make_unique<Keyboard>();

	_mouse = std::make_unique<Mouse>();
	_mouse->SetMode(Mouse::MODE_ABSOLUTE); // Use MODE_RELATIVE for displaying your own mouse pointer (like Diablo or Age of Empires)
	_mouse->SetWindow(window);

	_gamePad = std::make_unique<GamePad>();
}

void DirectXMain::OnNewAudioDevice()
{
    NG_AUDIO_ENGINE->update(-1);
}

#pragma region Frame Update
void DirectXMain::Tick()
{
    auto kb = _keyboard->GetState();
    _keys.Update(kb);
    for (unsigned short key : getAllSupportedKeys())
    {
        if (_keys.IsKeyPressed((DirectX::Keyboard::Keys)key))
        {
            KEYBOARD_INPUT_MANAGER->onInput(key);
        }
        else if (_keys.IsKeyReleased((DirectX::Keyboard::Keys)key))
        {
            KEYBOARD_INPUT_MANAGER->onInput(key, true);
        }
    }
    
    auto mouse = _mouse->GetState();
    if (mouse.positionMode == Mouse::MODE_ABSOLUTE)
    {
        CURSOR_INPUT_MANAGER->setCursorPosition(float(mouse.x), float(mouse.y));
        
        if (_isPointerPressed && mouse.leftButton)
        {
            CURSOR_INPUT_MANAGER->onInput(CursorEventType_DRAGGED, float(mouse.x), float(mouse.y));
        }
        else if (mouse.leftButton && !_isPointerPressed)
        {
            CURSOR_INPUT_MANAGER->onInput(CursorEventType_DOWN, float(mouse.x), float(mouse.y));
            
            _isPointerPressed = true;
        }
        else if (_isPointerPressed && !mouse.leftButton)
        {
            CURSOR_INPUT_MANAGER->onInput(CursorEventType_UP, float(mouse.x), float(mouse.y));
            
            _isPointerPressed = false;
        }
    }
    
    CURSOR_INPUT_MANAGER->onScroll(-mouse.scrollWheelValue);
    _mouse->ResetScrollWheelValue();
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        auto gamePadState = _gamePad->GetState(i);
        if (gamePadState.IsConnected())
        {
            _buttons[i].Update(gamePadState);
            
            if (_buttons[i].dpadRight == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_RIGHT, i, 1);
            }
            else if (_buttons[i].dpadRight == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_RIGHT, i);
            }
            if (_buttons[i].dpadUp == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_UP, i, 1);
            }
            else if (_buttons[i].dpadUp == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_UP, i);
            }
            if (_buttons[i].dpadLeft == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_LEFT, i, 1);
            }
            else if (_buttons[i].dpadLeft == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_LEFT, i);
            }
            if (_buttons[i].dpadDown == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_DOWN, i, 1);
            }
            else if (_buttons[i].dpadDown == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_DOWN, i);
            }
            
            if (_buttons[i].a == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, i, 1);
            }
            else if (_buttons[i].a == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, i);
            }
            if (_buttons[i].b == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, i, 1);
            }
            else if (_buttons[i].b == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, i);
            }
            if (_buttons[i].x == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, i, 1);
            }
            else if (_buttons[i].x == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, i);
            }
            if (_buttons[i].y == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_Y_BUTTON, i, 1);
            }
            else if (_buttons[i].y == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_Y_BUTTON, i);
            }
            
            if (_buttons[i].rightShoulder == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, i, 1);
            }
            else if (_buttons[i].rightShoulder == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, i);
            }
            if (_buttons[i].leftShoulder == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, i, 1);
            }
            else if (_buttons[i].leftShoulder == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, i);
            }
            
            if (_buttons[i].start == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, i, 1);
            }
            else if (_buttons[i].start == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, i);
            }
            if (_buttons[i].back == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, i, 1);
            }
            else if (_buttons[i].back == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, i);
            }
            
            if (_buttons[i].leftTrigger == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, i, gamePadState.triggers.left);
            }
            else if (_buttons[i].leftTrigger == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, i, 0);
            }
            if (_buttons[i].rightTrigger == GamePad::ButtonStateTracker::PRESSED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, i, 0, gamePadState.triggers.right);
            }
            else if (_buttons[i].rightTrigger == GamePad::ButtonStateTracker::RELEASED)
            {
                GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, i, 0, 0);
            }
            
            GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_LEFT, i, gamePadState.thumbSticks.leftX, gamePadState.thumbSticks.leftY);
            GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_RIGHT, i, gamePadState.thumbSticks.rightX, gamePadState.thumbSticks.rightY);
        }
    }
    
    int requestedAction = _engine->getRequestedAction();
    
    switch (requestedAction)
    {
        case REQUESTED_ACTION_EXIT:
            exitGame();
            return;
        case REQUESTED_ACTION_UPDATE:
            break;
        default:
            _engine->clearRequestedAction();
            break;
    }
    
    _timer.Tick([&]()
    {
        beginPixEvent(L"Update");
        _engine->update(_timer.GetElapsedSeconds());
        endPixEvent();
    });

    // Don't try to render anything before the first Update.
    if (_timer.GetFrameCount() == 0)
    {
        return;
    }
    
    beginPixEvent(L"Render");
    _engine->render();
    endPixEvent();
    
    // Show the new frame.
    beginPixEvent(L"Present");
    _deviceResources->Present();
    endPixEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void DirectXMain::OnActivated()
{
	// Game is becoming active window.

	_keys.Reset();
	_gamePad->Resume();

	for (DirectX::GamePad::ButtonStateTracker& button : _buttons)
	{
		button.Reset();
	}

	_engine->onResume();
}

void DirectXMain::OnDeactivated()
{
	// Game is becoming background window.

	_keys.Reset();
	_gamePad->Suspend();

	for (DirectX::GamePad::ButtonStateTracker& button : _buttons)
	{
		button.Reset();
	}

	_engine->onPause();
}

void DirectXMain::OnSuspending()
{
	// Game is being power-suspended (or minimized).

	_keys.Reset();
	_gamePad->Suspend();
	
	for (DirectX::GamePad::ButtonStateTracker& button : _buttons)
	{
		button.Reset();
	}

	_engine->onPause();
}

void DirectXMain::OnResuming()
{
	// Game is being power-resumed (or returning from minimize).

	_keys.Reset();
	_gamePad->Resume();
	
	for (DirectX::GamePad::ButtonStateTracker& button : _buttons)
	{
		button.Reset();
	}

	_timer.ResetElapsedTime();
	
	_engine->onResume();
}

void DirectXMain::OnWindowSizeChanged(int width, int height)
{
	if (!_deviceResources->WindowSizeChanged(width, height))
	{
		return;
	}

	CreateWindowSizeDependentResources();
}

// Properties
void DirectXMain::GetDefaultSize(int& width, int& height) const
{
	width = 800;
    height = 480;
}
#pragma endregion

#pragma region DirectX Resources
// These are the resources that depend on the device.
void DirectXMain::CreateDeviceDependentResources()
{
    _engine->createDeviceDependentResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void DirectXMain::CreateWindowSizeDependentResources()
{
    RECT outputSize = _deviceResources->GetOutputSize();
    LONG screenWidth = outputSize.right - outputSize.left;
    LONG screenHeight = outputSize.bottom - outputSize.top;

	_engine->createWindowSizeDependentResources(screenWidth, screenHeight);
}

void DirectXMain::beginPixEvent(PCWSTR pFormat)
{
    _deviceResources->PIXBeginEvent(pFormat);
}

void DirectXMain::endPixEvent()
{
    _deviceResources->PIXEndEvent();
}

void DirectXMain::OnDeviceLost()
{
	_engine->releaseDeviceDependentResources();
}

void DirectXMain::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion

DirectXMain::DirectXMain() : _engine(NULL), _dpi(0), _isPointerPressed(false), _isDeviceLost(false)
{
	_deviceResources = std::make_unique<DX::DirectXDeviceResources>();
	_deviceResources->RegisterDeviceNotify(this);

    DirectXRendererHelper::init(_deviceResources.get());
}

DirectXMain::~DirectXMain()
{
    _engine->releaseDeviceDependentResources();
    
	delete _engine;

	_deviceResources.reset();
}

void exitGame()
{
    PostQuitMessage(0);
}
