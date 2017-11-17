//
//  CursorConverter.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "CursorConverter.h"

#include "CursorEvent.h"

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
m_fTouchScreenWidth(160),
m_fTouchScreenHeight(90),
m_fCamWidth(1),
m_fCamHeight(1)
{
    // Empty
}

Vector2& CursorConverter::touchToWorld(CursorEvent& touchEvent)
{
#if TARGET_OS_IPHONE
    _cursor.set((touchEvent.getX() / m_fTouchScreenWidth) * m_fCamWidth, (1 - touchEvent.getY() / m_fTouchScreenHeight) * m_fCamHeight);
#elif TARGET_OS_OSX
    _cursor.set((touchEvent.getX() / m_fTouchScreenWidth) * m_fCamWidth, (touchEvent.getY() / m_fTouchScreenHeight) * m_fCamHeight);
#elif __ANDROID__
    _cursor.set((touchEvent.getX() / m_fTouchScreenWidth) * m_fCamWidth, (1 - touchEvent.getY() / m_fTouchScreenHeight) * m_fCamHeight);
#elif defined _WIN32
	_cursor.set(touchEvent.getX() / m_fTouchScreenWidth * m_fCamWidth, m_fCamHeight - (touchEvent.getY() / m_fTouchScreenHeight * m_fCamHeight));
#endif
    
    return _cursor;
}

void CursorConverter::setTouchScreenSize(int width, int height)
{
    m_fTouchScreenWidth = (float) width;
    m_fTouchScreenHeight = (float) height;
}

void CursorConverter::setCamSize(float camWidth, float camHeight)
{
    m_fCamWidth = camWidth;
    m_fCamHeight = camHeight;
}
