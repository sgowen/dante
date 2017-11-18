//
//  DirectXMain.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DirectXMain.h"

#include "Engine.h"

#include "DirectXManager.h"
#include "CursorInputManager.h"
#include "KeyboardInputManager.h"
#include "GamePadInputManager.h"
#include "MainAssets.h"
#include "FrameworkConstants.h"
#include "macros.h"
#include "NGAudioEngine.h"
#include "KeyboardLookup.h"

extern void exitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

DirectXMain::DirectXMain() : _engine(nullptr), _dpi(0), _isPointerPressed(false), _isDeviceLost(false)
{
    _deviceResources = std::make_unique<DX::DeviceResources>();
	_deviceResources->RegisterDeviceNotify(this);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), Rotation0);
	_isWindowsMobile = false;
#else
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), _deviceResources->GetOrientationTransform3D());
	Windows::System::Profile::AnalyticsVersionInfo^ api = Windows::System::Profile::AnalyticsInfo::VersionInfo;
	_isWindowsMobile = api->DeviceFamily->Equals("Windows.Mobile");
#endif

	MAIN_ASSETS->setUsingDesktopTextureSet(!_isWindowsMobile);
}

DirectXMain::~DirectXMain()
{
    delete _engine;

	_deviceResources.reset();
}

// Initialize the DirectX resources required to run.
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void DirectXMain::Initialize(Engine* engine, HWND window, int width, int height)
{
	_deviceResources->SetWindow(window, width, height);
#else
void DirectXMain::Initialize(Engine* engine, IUnknown* window, int width, int height, float dpi, DXGI_MODE_ROTATION rotation)
{
	_dpi = dpi;
	_deviceResources->SetWindow(window, width, height, rotation);
#endif
    
    _engine = engine;
    if (_engine->getRequestedAction() == REQUESTED_ACTION_EXIT)
    {
        exitGame();
        return;
    }

	_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

    int clampWidth = 1440;
    int clampHeight = 900;
	_deviceResources->CreateWindowSizeDependentResources(clampWidth, clampHeight);
	CreateWindowSizeDependentResources();

	_keyboard = std::make_unique<Keyboard>();

	_mouse = std::make_unique<Mouse>();
	_mouse->SetMode(Mouse::MODE_ABSOLUTE); // Use MODE_RELATIVE for displaying your own mouse pointer (like Diablo or Age of Empires)

	_gamePad = std::make_unique<GamePad>();

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	_mouse->SetWindow(window);
#else
	_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));
	_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));
#endif
}

void DirectXMain::OnNewAudioDevice()
{
    NG_AUDIO_ENGINE->update(-1);
}

#pragma region Frame Update
// Executes the basic game loop.
void DirectXMain::Tick()
{
    _timer.Tick([&]()
    {
        Update(_timer);
    });

    Render();
}

// Updates the world.
void DirectXMain::Update(DX::StepTimer const& timer)
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
		if (_isPointerPressed && mouse.leftButton)
		{
			SCREEN_INPUT_MANAGER->onTouch(CursorEventType_DRAGGED, float(mouse.x), float(mouse.y));
		}
		else if (mouse.leftButton && !_isPointerPressed)
		{
			SCREEN_INPUT_MANAGER->onTouch(CursorEventType_DOWN, float(mouse.x), float(mouse.y));

			_isPointerPressed = true;
		}
		else if (_isPointerPressed && !mouse.leftButton)
		{
			SCREEN_INPUT_MANAGER->onTouch(CursorEventType_UP, float(mouse.x), float(mouse.y));

			_isPointerPressed = false;
		}
	}

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

	beginPixEvent(L"Update");

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

	float elapsedTime = float(timer.GetElapsedSeconds());

	_engine->update(elapsedTime);

	endPixEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void DirectXMain::Render()
{
    // Don't try to render anything before the first Update.
    if (_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

	beginPixEvent(L"Render", _deviceResources.get());

	_engine->render();

	endPixEvent(_deviceResources.get());

    // Show the new frame.
	beginPixEvent(L"Present");
	_deviceResources->Present();
	endPixEvent();
}

// Helper method to clear the back buffers.
void DirectXMain::Clear()
{
	beginPixEvent(L"Clear", _deviceResources.get());

	// Clear the views.
	auto context = _deviceResources->GetD3DDeviceContext();
	auto renderTarget = _deviceResources->GetRenderTargetView();
	auto depthStencil = _deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = _deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	endPixEvent(_deviceResources.get());
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

	if (_isWindowsMobile
		&& _isDeviceLost)
	{
		OnDeviceRestored();
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

	if (_isWindowsMobile)
	{
		OnDeviceLost();

		_isDeviceLost = true;
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

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	// Empty
#else
	auto context = _deviceResources->GetD3DDeviceContext();
	context->ClearState();

	_deviceResources->Trim();
#endif

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

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void DirectXMain::OnWindowSizeChanged(int width, int height)
{
	if (!_deviceResources->WindowSizeChanged(width, height))
	{
		return;
	}

	CreateWindowSizeDependentResources();
}
#else
void DirectXMain::OnWindowSizeChanged(int width, int height, float dpi, DXGI_MODE_ROTATION rotation)
{
	_dpi = dpi;

	if (!_deviceResources->WindowSizeChanged(width, height, rotation))
	{
		return;
	}

	CreateWindowSizeDependentResources();
}

void DirectXMain::ValidateDevice()
{
	_deviceResources->ValidateDevice();
}
#endif

// Properties
void DirectXMain::GetDefaultSize(int& width, int& height) const
{
	width = 800;
    height = 600;
}
#pragma endregion

#pragma region DirectX Resources
// These are the resources that depend on the device.
void DirectXMain::CreateDeviceDependentResources()
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), Rotation0);
#else
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), _deviceResources->GetOrientationTransform3D());
#endif

	_engine->createDeviceDependentResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void DirectXMain::CreateWindowSizeDependentResources()
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), Rotation0);
#else
	DirectXManager::init(_deviceResources->GetD3DDevice(), _deviceResources->GetD3DDeviceContext(), _deviceResources->GetRenderTargetView(), _deviceResources->GetOrientationTransform3D());
#endif

	RECT outputSize = _deviceResources->GetOutputSize();
	LONG width = outputSize.right - outputSize.left;
	LONG height = outputSize.bottom - outputSize.top;
	LONG touchWidth = width;
	LONG touchHeight = height;

	if (D3DManager->isWindowsMobile())
	{
		touchWidth = height;
		touchHeight = width;
	}

    int clampWidth = 1440;
    int clampHeight = 900;
    
	width = width > clampWidth ? clampWidth : width;
	height = height > clampHeight ? clampHeight : height;

	_engine->createWindowSizeDependentResources(width, height, touchWidth, touchHeight);
}

void DirectXMain::beginPixEvent(PCWSTR pFormat, DX::DeviceResources* deviceResources)
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	UNUSED(deviceResources);

	_deviceResources->PIXBeginEvent(pFormat);
#else
	if (deviceResources)
	{
		auto context = deviceResources->GetD3DDeviceContext();
		PIXBeginEvent(context, PIX_COLOR_DEFAULT, pFormat);
	}
	else
	{
		PIXBeginEvent(PIX_COLOR_DEFAULT, pFormat);
	}
#endif
}

void DirectXMain::endPixEvent(DX::DeviceResources* deviceResources)
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	UNUSED(deviceResources);

	_deviceResources->PIXEndEvent();
#else
	if (deviceResources)
	{
		auto context = deviceResources->GetD3DDeviceContext();
		PIXEndEvent(context);
	}
	else
	{
		PIXEndEvent();
	}
#endif
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

void exitGame()
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	PostQuitMessage(0);
#else
	Windows::ApplicationModel::Core::CoreApplication::Exit();
#endif
}
