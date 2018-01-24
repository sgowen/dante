//
//  StudioInputManager.h
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioInputManager__
#define __noctisgames__StudioInputManager__

#include <framework/math/Vector2.h>

#include <string>

class StudioEngine;

enum StudioInputState
{
    SIS_NONE = 0,
    SIS_ESCAPE
};

enum StudioLayerFlag
{
    StudioLayerFlag_0 = 1 << 0,
    StudioLayerFlag_1 = 1 << 1,
    StudioLayerFlag_2 = 1 << 2,
    StudioLayerFlag_3 = 1 << 3,
    StudioLayerFlag_4 = 1 << 4,
    StudioLayerFlag_5 = 1 << 5,
    StudioLayerFlag_6 = 1 << 6,
    StudioLayerFlag_7 = 1 << 7,
    StudioLayerFlag_8 = 1 << 8,
    
    StudioLayerFlag_All = StudioLayerFlag_0 | StudioLayerFlag_1 | StudioLayerFlag_2 | StudioLayerFlag_3 | StudioLayerFlag_4 | StudioLayerFlag_5 | StudioLayerFlag_6 | StudioLayerFlag_7 | StudioLayerFlag_8
};

class StudioInputManager
{
    friend class StudioRenderer;
    
public:
    static void create();
    static StudioInputManager* getInstance();
    static void destroy();
    
    void update(StudioEngine* engine);
    void setLiveInputMode(bool isLiveMode);
    bool isLiveMode();
    bool isTimeToProcessInput();
    void onInputProcessed();
    int getMenuState();
    std::string& getLiveInputRef();
    std::string getLiveInput();
    
private:
    static StudioInputManager* s_instance;
    
    Vector2 _downCursor;
    Vector2 _dragCursor;
    Vector2 _deltaCursor;
    Vector2 _cursor;
    Vector2 _upCursor;
    std::string _liveInput;
    int _inputState;
    bool _isLiveMode;
    bool _isTimeToProcessInput;
    bool _isControl;
    float _rawScrollValue;
    int _scrollValue;
    int _lastScrollValue;
    int _layerFlag;
    bool isPanningUp;
    bool isPanningDown;
    bool isPanningRight;
    bool isPanningLeft;
    
    void updateCamera(StudioEngine* engine);
    void resetCamera();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    StudioInputManager();
    StudioInputManager(const StudioInputManager&);
    StudioInputManager& operator=(const StudioInputManager&);
};

#endif /* defined(__noctisgames__StudioInputManager__) */
