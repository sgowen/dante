//
//  CursorConverter.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/input/CursorConverter.h"

#include "framework/input/CursorEvent.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

CursorConverter* CursorConverter::getInstance()
{
    static CursorConverter instance = CursorConverter();
    return &instance;
}

CursorConverter::CursorConverter() :
_cursor(),
_touchScreenWidth(160),
_touchScreenHeight(90),
_camWidth(1),
_camHeight(1)
{
    // Empty
}

Vector2& CursorConverter::touchToWorld(CursorEvent& touchEvent)
{
#if TARGET_OS_IPHONE
    _cursor.set((touchEvent.getX() / _touchScreenWidth) * _camWidth, (1 - touchEvent.getY() / _touchScreenHeight) * _camHeight);
#elif TARGET_OS_OSX
    _cursor.set((touchEvent.getX() / _touchScreenWidth) * _camWidth, (touchEvent.getY() / _touchScreenHeight) * _camHeight);
#elif __ANDROID__
    _cursor.set((touchEvent.getX() / _touchScreenWidth) * _camWidth, (1 - touchEvent.getY() / _touchScreenHeight) * _camHeight);
#elif defined _WIN32
	_cursor.set(touchEvent.getX() / _touchScreenWidth * _camWidth, _camHeight - (touchEvent.getY() / _touchScreenHeight * _camHeight));
#endif
    
    return _cursor;
}

void CursorConverter::setTouchScreenSize(int width, int height)
{
    _touchScreenWidth = (float) width;
    _touchScreenHeight = (float) height;
}

void CursorConverter::setCamSize(float camWidth, float camHeight)
{
    _camWidth = camWidth;
    _camHeight = camHeight;
}
