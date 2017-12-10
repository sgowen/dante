//
//  CursorConverter.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CursorConverter__
#define __noctisgames__CursorConverter__

#include "framework/math/Vector2.h"

#define CURSOR_CONVERTER (CursorConverter::getInstance())

class CursorEvent;

class CursorConverter
{
public:
    static CursorConverter* getInstance();
    
    Vector2& touchToWorld(CursorEvent& touchEvent);
    
    void setCursorSize(int width, int height);
    
    void setCamSize(float camWidth, float camHeight);
    
private:
    Vector2 _cursor;
    float _cursorWidth;
    float _cursorHeight;
    float _camWidth;
    float _camHeight;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    CursorConverter();
    CursorConverter(const CursorConverter&);
    CursorConverter& operator=(const CursorConverter&);
};

#endif /* defined(__noctisgames__CursorConverter__) */
