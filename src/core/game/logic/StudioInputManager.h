//
//  StudioInputManager.h
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StudioInputManager__
#define __noctisgames__StudioInputManager__

#include <string>

class MainInputState;

class StudioInputManager
{
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
    std::string _liveInput;
    int _menuState;
    bool _isLiveMode;
    bool _isTimeToProcessInput;
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    StudioInputManager();
    StudioInputManager(const StudioInputManager&);
    StudioInputManager& operator=(const StudioInputManager&);
};

#endif /* defined(__noctisgames__StudioInputManager__) */
