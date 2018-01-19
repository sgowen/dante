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
_cursorWidth(160),
_cursorHeight(90),
_camWidth(1),
_camHeight(1)
{
    // Empty
}

Vector2& CursorConverter::convert(CursorEvent& cursorEvent)
{
#if TARGET_OS_IPHONE
    _cursor.set((cursorEvent.getX() / _cursorWidth) * _camWidth, (1 - cursorEvent.getY() / _cursorHeight) * _camHeight);
#elif TARGET_OS_OSX
    _cursor.set((cursorEvent.getX() / _cursorWidth) * _camWidth, (cursorEvent.getY() / _cursorHeight) * _camHeight);
#elif __ANDROID__
    _cursor.set((cursorEvent.getX() / _cursorWidth) * _camWidth, (1 - cursorEvent.getY() / _cursorHeight) * _camHeight);
#elif defined _WIN32
	_cursor.set(cursorEvent.getX() / _cursorWidth * _camWidth, _camHeight - (cursorEvent.getY() / _cursorHeight * _camHeight));
#endif
    
    return _cursor;
}

Vector2& CursorConverter::convert(Vector2& cursorPosition)
{
#if TARGET_OS_IPHONE
    _cursor.set((cursorPosition.getX() / _cursorWidth) * _camWidth, (1 - cursorPosition.getY() / _cursorHeight) * _camHeight);
#elif TARGET_OS_OSX
    _cursor.set((cursorPosition.getX() / _cursorWidth) * _camWidth, (cursorPosition.getY() / _cursorHeight) * _camHeight);
#elif __ANDROID__
    _cursor.set((cursorPosition.getX() / _cursorWidth) * _camWidth, (1 - cursorPosition.getY() / _cursorHeight) * _camHeight);
#elif defined _WIN32
    _cursor.set(cursorPosition.getX() / _cursorWidth * _camWidth, _camHeight - (cursorPosition.getY() / _cursorHeight * _camHeight));
#endif
    
    return _cursor;
}

void CursorConverter::setCursorSize(int width, int height)
{
    _cursorWidth = (float) width;
    _cursorHeight = (float) height;
}

void CursorConverter::setCamSize(float camWidth, float camHeight)
{
    _camWidth = camWidth;
    _camHeight = camHeight;
}
