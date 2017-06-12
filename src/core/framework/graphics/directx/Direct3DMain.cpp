//
//  Direct3DMain.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DMain.h"

#include "IEngine.h"

#include "Direct3DManager.h"
#include "ScreenInputManager.h"
#include "KeyboardInputManager.h"
#include "GamePadInputManager.h"
#include "MainAssets.h"
#include "FrameworkConstants.h"
#include "macros.h"
#include "NGAudioEngine.h"

extern void exitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Direct3DMain::Direct3DMain() : m_engine(nullptr), m_fDPI(0), m_isPointerPressed(false), m_isDeviceLost(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), Rotation0);
	m_isWindowsMobile = false;
#else
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), m_deviceResources->GetOrientationTransform3D());
	Windows::System::Profile::AnalyticsVersionInfo^ api = Windows::System::Profile::AnalyticsInfo::VersionInfo;
	m_isWindowsMobile = api->DeviceFamily->Equals("Windows.Mobile");
#endif

	MAIN_ASSETS->setUsingDesktopTextureSet(!m_isWindowsMobile);
}

Direct3DMain::~Direct3DMain()
{
    delete m_engine;

	m_deviceResources.reset();
}

// Initialize the Direct3D resources required to run.
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void Direct3DMain::Initialize(IEngine* engine, HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);
#else
void Direct3DMain::Initialize(IEngine* engine, IUnknown* window, int width, int height, float dpi, DXGI_MODE_ROTATION rotation)
{
	m_fDPI = dpi;
	m_deviceResources->SetWindow(window, width, height, rotation);
#endif
    
    m_engine = engine;
    if (m_engine->getRequestedAction() == REQUESTED_ACTION_EXIT)
    {
        exitGame();
        return;
    }

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

    int clampWidth = 1440;
    int clampHeight = 900;
	m_deviceResources->CreateWindowSizeDependentResources(clampWidth, clampHeight);
	CreateWindowSizeDependentResources();

	m_keyboard = std::make_unique<Keyboard>();

	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetMode(Mouse::MODE_ABSOLUTE); // Use MODE_RELATIVE for displaying your own mouse pointer (like Diablo or Age of Empires)

	m_gamePad = std::make_unique<GamePad>();

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	m_mouse->SetWindow(window);
#else
	m_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));
	m_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));
#endif
}

void Direct3DMain::OnNewAudioDevice()
{
    NG_AUDIO_ENGINE->update(-1);
}

#pragma region Frame Update
// Executes the basic game loop.
void Direct3DMain::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Direct3DMain::Update(DX::StepTimer const& timer)
{
	auto kb = m_keyboard->GetState();
	m_keys.Update(kb);
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Right))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_RIGHT);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Right))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_RIGHT, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Up))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_UP);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Up))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_UP, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Left))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_LEFT);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Left))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_LEFT, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Down))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_DOWN);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Down))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ARROW_KEY_DOWN, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::W))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_W);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::W))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_W, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::A))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_A);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::A))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_A, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::S))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_S);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::S))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_S, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::D))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_D);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::D))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_D, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::V))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_V);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::V))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_V, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::M))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_M);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::M))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_M, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::P))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_P);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::P))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_P, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Enter))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ENTER);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Enter))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_ENTER, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Space))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_SPACE);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Space))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_SPACE, true);
	}
	if (m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Back)
		|| m_keys.IsKeyPressed(DirectX::Keyboard::Keys::Delete))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_BACK);
	}
	else if (m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Back)
		|| m_keys.IsKeyReleased(DirectX::Keyboard::Keys::Delete))
	{
		KEYBOARD_INPUT_MANAGER->onInput(KeyboardEventType_BACK, true);
	}

	auto mouse = m_mouse->GetState();
	if (mouse.positionMode == Mouse::MODE_ABSOLUTE)
	{
		if (m_isPointerPressed && mouse.leftButton)
		{
			SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DRAGGED, float(mouse.x), float(mouse.y));
		}
		else if (mouse.leftButton && !m_isPointerPressed)
		{
			SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DOWN, float(mouse.x), float(mouse.y));

			m_isPointerPressed = true;
		}
		else if (m_isPointerPressed && !mouse.leftButton)
		{
			SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_UP, float(mouse.x), float(mouse.y));

			m_isPointerPressed = false;
		}
	}

	auto gamePadState = m_gamePad->GetState(0);
	if (gamePadState.IsConnected())
	{
        m_buttons.Update(gamePadState);

		if (m_buttons.dpadRight == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_RIGHT, 0, 1);
		}
		else if (m_buttons.dpadRight == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_RIGHT, 0);
		}
		if (m_buttons.dpadUp == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_UP, 0, 1);
		}
		else if (m_buttons.dpadUp == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_UP, 0);
		}
		if (m_buttons.dpadLeft == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_LEFT, 0, 1);
		}
		else if (m_buttons.dpadLeft == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_LEFT, 0);
		}
		if (m_buttons.dpadDown == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_DOWN, 0, 1);
		}
		else if (m_buttons.dpadDown == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_D_PAD_DOWN, 0);
		}
        
		if (m_buttons.a == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, 0, 1);
		}
		else if (m_buttons.a == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, 0);
		}
		if (m_buttons.b == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, 0, 1);
		}
		else if (m_buttons.b == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, 0);
		}
		if (m_buttons.x == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, 0, 1);
		}
		else if (m_buttons.x == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, 0);
		}
		if (m_buttons.y == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_Y_BUTTON, 0, 1);
		}
		else if (m_buttons.y == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_Y_BUTTON, 0);
		}

		if (m_buttons.rightShoulder == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, 0, 1);
		}
		else if (m_buttons.rightShoulder == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, 0);
		}
		if (m_buttons.leftShoulder == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, 0, 1);
		}
		else if (m_buttons.leftShoulder == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, 0);
		}

		if (m_buttons.start == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, 0, 1);
		}
		else if (m_buttons.start == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, 0);
		}
		if (m_buttons.back == GamePad::ButtonStateTracker::PRESSED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, 0, 1);
		}
		else if (m_buttons.back == GamePad::ButtonStateTracker::RELEASED)
		{
			GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, 0);
		}
        
		GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, 0, gamePadState.triggers.left, gamePadState.triggers.right);
		GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_LEFT, 0, gamePadState.thumbSticks.leftX, gamePadState.thumbSticks.leftY);
		GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_RIGHT, 0, gamePadState.thumbSticks.rightX, gamePadState.thumbSticks.rightY);
	}

	beginPixEvent(L"Update");

	int requestedAction = m_engine->getRequestedAction();

	switch (requestedAction)
	{
	case REQUESTED_ACTION_EXIT:
		exitGame();
		return;
	case REQUESTED_ACTION_UPDATE:
		break;
	default:
		m_engine->clearRequestedAction();
		break;
	}

	float elapsedTime = float(timer.GetElapsedSeconds());

	m_engine->update(elapsedTime);

	endPixEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Direct3DMain::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

	beginPixEvent(L"Render", m_deviceResources.get());

	m_engine->render();

	endPixEvent(m_deviceResources.get());

    // Show the new frame.
	beginPixEvent(L"Present");
	m_deviceResources->Present();
	endPixEvent();
}

// Helper method to clear the back buffers.
void Direct3DMain::Clear()
{
	beginPixEvent(L"Clear", m_deviceResources.get());

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	endPixEvent(m_deviceResources.get());
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Direct3DMain::OnActivated()
{
	// Game is becoming active window.

	m_keys.Reset();
	m_gamePad->Resume();
	m_buttons.Reset();

	if (m_isWindowsMobile
		&& m_isDeviceLost)
	{
		OnDeviceRestored();
	}

	m_engine->onResume();
}

void Direct3DMain::OnDeactivated()
{
	// Game is becoming background window.

	m_keys.Reset();
	m_gamePad->Suspend();
	m_buttons.Reset();

	if (m_isWindowsMobile)
	{
		OnDeviceLost();

		m_isDeviceLost = true;
	}

	m_engine->onPause();
}

void Direct3DMain::OnSuspending()
{
	// Game is being power-suspended (or minimized).

	m_keys.Reset();
	m_gamePad->Suspend();
	m_buttons.Reset();

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	// Empty
#else
	auto context = m_deviceResources->GetD3DDeviceContext();
	context->ClearState();

	m_deviceResources->Trim();
#endif

	m_engine->onPause();
}

void Direct3DMain::OnResuming()
{
	// Game is being power-resumed (or returning from minimize).

	m_keys.Reset();
	m_gamePad->Resume();
	m_buttons.Reset();

	m_timer.ResetElapsedTime();
	
	m_engine->onResume();
}

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void Direct3DMain::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
	{
		return;
	}

	CreateWindowSizeDependentResources();
}
#else
void Direct3DMain::OnWindowSizeChanged(int width, int height, float dpi, DXGI_MODE_ROTATION rotation)
{
	m_fDPI = dpi;

	if (!m_deviceResources->WindowSizeChanged(width, height, rotation))
	{
		return;
	}

	CreateWindowSizeDependentResources();
}

void Direct3DMain::ValidateDevice()
{
	m_deviceResources->ValidateDevice();
}
#endif

// Properties
void Direct3DMain::GetDefaultSize(int& width, int& height) const
{
	width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Direct3DMain::CreateDeviceDependentResources()
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), Rotation0);
#else
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), m_deviceResources->GetOrientationTransform3D());
#endif

	m_engine->createDeviceDependentResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Direct3DMain::CreateWindowSizeDependentResources()
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	static const XMFLOAT4X4 Rotation0(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), Rotation0);
#else
	Direct3DManager::init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_deviceResources->GetRenderTargetView(), m_deviceResources->GetOrientationTransform3D());
#endif

	RECT outputSize = m_deviceResources->GetOutputSize();
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

	m_engine->createWindowSizeDependentResources(width, height, touchWidth, touchHeight);
}

void Direct3DMain::beginPixEvent(PCWSTR pFormat, DX::DeviceResources* deviceResources)
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	UNUSED(deviceResources);

	m_deviceResources->PIXBeginEvent(pFormat);
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

void Direct3DMain::endPixEvent(DX::DeviceResources* deviceResources)
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	UNUSED(deviceResources);

	m_deviceResources->PIXEndEvent();
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

void Direct3DMain::OnDeviceLost()
{
	m_engine->releaseDeviceDependentResources();
}

void Direct3DMain::OnDeviceRestored()
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
