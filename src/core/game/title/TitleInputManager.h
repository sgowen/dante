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

enum TitleInputManagerState
{
    TIMS_NONE = 0,
    TIMS_ENTER_STUDIO,
    TIMS_ACTIVATE_STEAM,
    TIMS_DEACTIVATE_STEAM,
    TIMS_START_SERVER,
    TIMS_JOIN_LOCAL_SERVER,
    TIMS_STEAM_REFRESH_LAN_SERVERS,
    TIMS_STEAM_REFRESH_INTERNET_SERVERS,
    TIMS_STEAM_JOIN_SERVER_1,
    TIMS_STEAM_JOIN_SERVER_2,
    TIMS_STEAM_JOIN_SERVER_3,
    TIMS_STEAM_JOIN_SERVER_4,
    TIMS_ESCAPE
};

class TitleEngine;

class TitleInputManager
{
public:
    static void create();
    static TitleInputManager* getInstance();
    static void destroy();
    
    void update();
    void setLiveInputMode(bool isLiveMode);
    bool isLiveMode();
    bool isTimeToProcessInput();
    void onInputProcessed();
    int getMenuState();
    std::string& getLiveInputRef();
    std::string getLiveInput();
    
private:
    static TitleInputManager* s_instance;
    
    std::string _liveInput;
    int _inputState;
    bool _isLiveMode;
    bool _isTimeToProcessInput;
    TitleEngine* _engine;
    
    bool isTimeToSampleInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    TitleInputManager();
    TitleInputManager(const TitleInputManager&);
    TitleInputManager& operator=(const TitleInputManager&);
};

#endif /* defined(__noctisgames__TitleInputManager__) */
