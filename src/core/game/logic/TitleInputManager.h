//
//  TitleInputManager.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TitleInputManager__
#define __noctisgames__TitleInputManager__

#include <string>

class MainInputState;

class TitleInputManager
{
public:
    static TitleInputManager* getInstance();
    
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
    TitleInputManager();
    TitleInputManager(const TitleInputManager&);
    TitleInputManager& operator=(const TitleInputManager&);
};

#endif /* defined(__noctisgames__TitleInputManager__) */
