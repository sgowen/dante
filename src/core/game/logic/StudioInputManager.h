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

class MainInputState;

enum StudioInputState
{
    SIS_NONE = 0,
    SIS_ESCAPE
};

#define STUDIO_INPUT StudioInputManager::getInstance()

class StudioInputManager
{
    friend class StudioRenderer;
    
public:
    static StudioInputManager* getInstance();
    
    void update();
    void setLiveInputMode(bool isLiveMode);
    bool isLiveMode();
    bool isTimeToProcessInput();
    void onInputProcessed();
    int getMenuState();
    std::string& getLiveInputRef();
    std::string getLiveInput();
    
private:
    Vector2 _downCursor;
    Vector2 _dragCursor;
    Vector2 _upCursor;
    std::string _liveInput;
    int _inputState;
    bool _isLiveMode;
    bool _isTimeToProcessInput;
    bool _isControl;
    int _scrollValue;
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    StudioInputManager();
    StudioInputManager(const StudioInputManager&);
    StudioInputManager& operator=(const StudioInputManager&);
};

#endif /* defined(__noctisgames__StudioInputManager__) */
