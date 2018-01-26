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
#include <vector>

class StudioEngine;
class Entity;

enum StudioInputState
{
    SIS_NONE = 0,
    SIS_ESCAPE
};

class StudioInputManager
{
    friend class StudioRenderer;
    
public:
    static void create();
    static StudioInputManager* getInstance();
    static void destroy();
    
    void setEngine(StudioEngine* inValue);
    int getMenuState();
    void update();
    
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
    bool _isPanningUp;
    bool _isPanningDown;
    bool _isPanningRight;
    bool _isPanningLeft;
    int _selectionIndex;
    float _rawSelectionIndex;
    float _selectionIndexDir;
    std::vector<Entity*> _entities;
    Entity* _activeEntity;
    Entity* _lastActiveEntity;
    Vector2 _activeEntityCursor;
    Vector2 _activeEntityDeltaCursor;
    StudioEngine* _engine;
    bool _isDraggingActiveEntityOverDeleteZone;
    bool _hasTouchedScreen;
    
    void updateCamera();
    void resetCamera();
    
    void handleDefaultInput();
    void handleEntitiesInput();
    void handleLoadMapDialogInput();
    
    void onMapLoaded();
    void onEntityAdded(Entity* e);
    void onEntityRemoved(Entity* e);
    Entity* getEntityAtPosition(float x, float y);
    bool entityExistsAtPosition(Entity* e, float x, float y);
    
    void setLiveInputMode(bool isLiveMode);
    bool isLiveMode();
    bool isTimeToProcessInput();
    void onInputProcessed();
    std::string& getLiveInputRef();
    std::string getLiveInput();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    StudioInputManager();
    StudioInputManager(const StudioInputManager&);
    StudioInputManager& operator=(const StudioInputManager&);
};

#endif /* defined(__noctisgames__StudioInputManager__) */
