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

enum TitleInputState
{
    TIS_NONE = 0,
    TIS_ENTER_STUDIO,
    TIS_ACTIVATE_STEAM,
    TIS_DEACTIVATE_STEAM,
    TIS_START_SERVER,
    TIS_JOIN_LOCAL_SERVER,
    TIS_STEAM_REFRESH_LAN_SERVERS,
    TIS_STEAM_REFRESH_INTERNET_SERVERS,
    TIS_STEAM_JOIN_SERVER_1,
    TIS_STEAM_JOIN_SERVER_2,
    TIS_STEAM_JOIN_SERVER_3,
    TIS_STEAM_JOIN_SERVER_4,
    TIS_ESCAPE
};

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
