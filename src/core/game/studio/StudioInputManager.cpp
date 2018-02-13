//
//  StudioInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/studio/StudioInputManager.h>

#include <game/studio/StudioEngine.h>
#include <framework/entity/Entity.h>

#include <framework/util/Timing.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/input/KeyboardInputManager.h>
#include <framework/input/KeyboardEvent.h>
#include <framework/input/GamePadInputManager.h>
#include <framework/input/GamePadEvent.h>
#include <framework/util/Constants.h>
#include <framework/input/KeyboardLookup.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/util/Constants.h>
#include <game/studio/StudioRenderer.h>
#include <game/logic/World.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityMapper.h>
#include <framework/math/OverlapTester.h>
#include <framework/math/NGRect.h>
#include <game/logic/GameConfig.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <game/logic/Server.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/macros.h>

#include <sstream>

StudioInputManager* StudioInputManager::s_instance = NULL;
uint32_t StudioInputManager::s_testMap = 'TEST';

void StudioInputManager::create()
{
    assert(!s_instance);
    
    s_instance = new StudioInputManager();
}

StudioInputManager * StudioInputManager::getInstance()
{
    return s_instance;
}

void StudioInputManager::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void StudioInputManager::setEngine(StudioEngine* inValue)
{
    _engine = inValue;
}

int StudioInputManager::getMenuState()
{
    return _inputState;
}

void StudioInputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _inputState = SIMS_NONE;
    
    uint32_t state = _engine->_state;
    if (state & StudioEngineState_TextInput)
    {
        handleTextInput();
    }
    else if (state & StudioEngineState_TestSession)
    {
        handleTestSessionInput();
    }
    else if (state & StudioEngineState_DisplayLoadMapDialog)
    {
        handleLoadMapDialogInput();
    }
    else if (state & StudioEngineState_DisplayEntities)
    {
        handleEntitiesInput();
    }
    else
    {
        handleDefaultInput();
        updateCamera();
    }
}

void StudioInputManager::handleDefaultInput()
{
    _rawScrollValue = clamp(_rawScrollValue + CURSOR_INPUT_MANAGER->getScrollWheelValue(), 16, 1);
    CURSOR_INPUT_MANAGER->resetScrollValue();
    _scrollValue = clamp(_rawScrollValue, 16, 1);
    CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth * _scrollValue, GM_CFG->_camHeight * _scrollValue);
    
    for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
    {
        CursorEvent& e = *(*i);
        Vector2& c = CURSOR_CONVERTER->convert(e);
        switch (e.getType())
        {
            case CursorEventType_DOWN:
            {
                _hasTouchedScreen = true;
                
                _downCursor.set(c);
                _dragCursor.set(c);
                _deltaCursor.set(0, 0);
                
                _activeEntityDeltaCursor.set(0, 0);
                _activeEntityCursor.set(_cursor);
                _activeEntityCursor += _downCursor;
                
                if (e.isAlt())
                {
                    if (_lastActiveEntity)
                    {
                        onEntityRemoved(_lastActiveEntity);
                        _engine->_world->mapRemoveEntity(_lastActiveEntity);
                        
                        _activeEntity = NULL;
                        _lastActiveEntity = NULL;
                    }
                }
                else
                {
                    _activeEntity = getEntityAtPosition(_activeEntityCursor.getX(), _activeEntityCursor.getY());
                    if (_activeEntity)
                    {
                        _activeEntityDeltaCursor.set(_activeEntity->getPosition().x, _activeEntity->getPosition().y);
                        _activeEntityDeltaCursor -= _activeEntityCursor;
                        
                        _lastActiveEntity = NULL;
                    }
                }
            }
                continue;
            case CursorEventType_DRAGGED:
            {
                Vector2 delta = c;
                delta -= _dragCursor;
                _dragCursor.set(c);
                _deltaCursor.set(delta);
                
                if (_activeEntity)
                {
                    _activeEntityCursor += _deltaCursor;
                    Vector2 entityPos = _activeEntityCursor;
                    entityPos += _activeEntityDeltaCursor;
                    _activeEntity->setPosition(b2Vec2(entityPos.getX(), entityPos.getY()));
                }
            }
                continue;
            case CursorEventType_UP:
            {
                _upCursor.set(c);
                _deltaCursor.set(0, 0);
                
                if (_activeEntity)
                {
                    const b2Vec2& position = _activeEntity->getPosition();
                    float width = _activeEntity->getWidth();
                    float height = _activeEntity->getHeight();
                    float x = clamp(position.x, FLT_MAX, width / 2);
                    float y = clamp(position.y, FLT_MAX, height / 2);
                    x = floor(x);
                    y = floor(y);
                    _activeEntity->setPosition(b2Vec2(x, y));
                    
                    _lastActiveEntity = _activeEntity;
                    
                    _activeEntity = NULL;
                }
            }
                break;
            default:
                break;
        }
    }
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent& e = *(*i);
        switch (e.getKey())
        {
            case NG_KEY_CMD:
            case NG_KEY_CTRL:
                _isControl = e.isPressed();
                continue;
            case NG_KEY_ZERO:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer0 : 0;
                continue;
            case NG_KEY_ONE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer1 : 0;
                continue;
            case NG_KEY_TWO:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer2 : 0;
                continue;
            case NG_KEY_THREE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer3 : 0;
                continue;
            case NG_KEY_FOUR:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer4 : 0;
                continue;
            case NG_KEY_FIVE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer5 : 0;
                continue;
            case NG_KEY_SIX:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer6 : 0;
                continue;
            case NG_KEY_SEVEN:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer7 : 0;
                continue;
            case NG_KEY_EIGHT:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer8 : 0;
                continue;
            case NG_KEY_NINE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer9 : 0;
                continue;
            case NG_KEY_D:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayTypes : 0;
                continue;
            case NG_KEY_ARROW_LEFT:
                _isPanningLeft = e.isPressed();
                continue;
            case NG_KEY_ARROW_RIGHT:
                _isPanningRight = e.isPressed();
                continue;
            case NG_KEY_ARROW_DOWN:
                _isPanningDown = e.isPressed();
                continue;
            case NG_KEY_ARROW_UP:
                _isPanningUp = e.isPressed();
                continue;
            case NG_KEY_C:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayControls : 0;
                continue;
            case NG_KEY_A:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayAssets : 0;
                if (e.isDown())
                {
                    _engine->_renderer->displayToast("Assets Management not yet implemented...");
                }
                continue;
            case NG_KEY_E:
                if (e.isDown())
                {
                    if (_engine->_world->isMapLoaded())
                    {
                        _engine->_state |= StudioEngineState_DisplayEntities;
                    }
                    else
                    {
                        _engine->_renderer->displayToast("Load a Map first!");
                    }
                }
                continue;
            case NG_KEY_R:
                if (e.isDown())
                {
                    resetCamera();
                }
                continue;
            case NG_KEY_N:
                _engine->_state |= e.isDown() ? StudioEngineState_DisplayNewMapDialog : 0;
                if (e.isDown())
                {
                    _engine->_renderer->displayToast("New Map not yet implemented...");
                }
                continue;
            case NG_KEY_L:
                _engine->_state |= e.isDown() ? StudioEngineState_DisplayLoadMapDialog : 0;
                continue;
            case NG_KEY_X:
                if (e.isDown())
                {
                    _engine->_world->clear();
                    onMapLoaded();
                }
                continue;
            case NG_KEY_T:
            {
                if (e.isDown())
                {
                    if (_engine->_world->getDynamicEntities().size() > MAX_NUM_DYNAMIC_ENTITIES)
                    {
                        _engine->_renderer->displayToast(StringUtil::format("Cannot have more than %d dynamic entities!", MAX_NUM_DYNAMIC_ENTITIES));
                    }
                    else
                    {
                        if (NG_CLIENT)
                        {
                            NetworkManagerClient::destroy();
                        }
                        
                        if (Server::getInstance())
                        {
                            Server::destroy();
                        }
                        
                        Server::create(ServerFlag_TestSession, &s_testMap);
                        assert(NG_SERVER);
                        
                        _engine->_world->saveMapAs(s_testMap);
                        _engine->_state |= StudioEngineState_TestSession;
                    }
                }
            }
                continue;
            case NG_KEY_S:
                if (e.isDown())
                {
                    if (_engine->_world->getDynamicEntities().size() > MAX_NUM_DYNAMIC_ENTITIES)
                    {
                        _engine->_renderer->displayToast(StringUtil::format("Cannot have more than %d dynamic entities!", MAX_NUM_DYNAMIC_ENTITIES));
                    }
                    else
                    {
                        if (_isControl)
                        {
                            _engine->_state |= e.isDown() ? StudioEngineState_DisplaySaveMapAsDialog : 0;
                            _engine->_renderer->displayToast("Save As not yet implemented...");
                        }
                        else
                        {
                            _engine->_world->saveMap();
                            _engine->_renderer->displayToast(StringUtil::format("%s saved!", _engine->_world->getMapName().c_str()).c_str());
                        }
                    }
                }
                continue;
            case NG_KEY_P:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayParallax : 0;
                continue;
            case NG_KEY_B:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayBox2D : 0;
                continue;
            case NG_KEY_G:
                _engine->_state ^= e.isDown() ? StudioEngineState_DisplayGrid : 0;
                continue;
            case NG_KEY_ESCAPE:
                _inputState = e.isDown() ? SIMS_ESCAPE : SIMS_NONE;
                continue;
            default:
                continue;
        }
    }
}

void StudioInputManager::handleTextInput()
{
    std::stringstream ss;
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent* keyboardEvent = (*i);
        if (isKeySupported(keyboardEvent->getKey()))
        {
            if (keyboardEvent->isDown()
                && !keyboardEvent->isHeld())
            {
                if (keyboardEvent->getKey() == NG_KEY_CARRIAGE_RETURN)
                {
                    _isTimeToProcessInput = true;
                    return;
                }
                else if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                {
                    return;
                }
                else if (keyboardEvent->getKey() == NG_KEY_BACK_SPACE
                         || keyboardEvent->getKey() == NG_KEY_DELETE)
                {
                    std::string s = ss.str();
                    _liveInput += s;
                    if (_liveInput.end() > _liveInput.begin())
                    {
                        _liveInput.erase(_liveInput.end() - 1, _liveInput.end());
                    }
                    return;
                }
                else
                {
                    char key = charForKey(keyboardEvent->getKey());
                    ss << StringUtil::format("%c", key);
                }
            }
            else if (keyboardEvent->isUp())
            {
                if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                {
                    _inputState = SIMS_ESCAPE;
                    return;
                }
            }
        }
    }
    
    std::string s = ss.str();
    _liveInput += s;
    if (_liveInput.length() > 16)
    {
        int sub = static_cast<int>(_liveInput.length()) - 16;
        _liveInput.erase(_liveInput.end() - sub, _liveInput.end());
    }
}

void StudioInputManager::handleTestSessionInput()
{
    /// TODO
}

void StudioInputManager::handleEntitiesInput()
{
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent& e = *(*i);
        switch (e.getKey())
        {
            case NG_KEY_ZERO:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer0 : 0;
                continue;
            case NG_KEY_ONE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer1 : 0;
                continue;
            case NG_KEY_TWO:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer2 : 0;
                continue;
            case NG_KEY_THREE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer3 : 0;
                continue;
            case NG_KEY_FOUR:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer4 : 0;
                continue;
            case NG_KEY_FIVE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer5 : 0;
                continue;
            case NG_KEY_SIX:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer6 : 0;
                continue;
            case NG_KEY_SEVEN:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer7 : 0;
                continue;
            case NG_KEY_EIGHT:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer8 : 0;
                continue;
            case NG_KEY_NINE:
                _engine->_state ^= e.isDown() ? StudioEngineState_Layer9 : 0;
                continue;
            case NG_KEY_ARROW_DOWN:
                _selectionIndexDir = e.isPressed() ? 0.25f : 0;
                continue;
            case NG_KEY_ARROW_UP:
                _selectionIndexDir = e.isPressed() ? -0.25f : 0;
                continue;
            case NG_KEY_CARRIAGE_RETURN:
            {
                if (e.isDown())
                {
                    const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
                    EntityDef* entityDef = entityDescriptors[_selectionIndex];
                    if (entityDef->type == 'ROBT')
                    {
                        _engine->_renderer->displayToast("Players can only be spawned by the server...");
                        continue;
                    }
                    
                    float spawnX = clamp(GM_CFG->_camWidth * _scrollValue / 2 + _cursor.getX(), FLT_MAX, entityDef->width / 2.0f);
                    float spawnY = clamp(GM_CFG->_camHeight * _scrollValue / 2 + _cursor.getY(), FLT_MAX, entityDef->height / 2.0f);
                    Entity* e = EntityMapper::getInstance()->createEntityFromDef(entityDef, floor(spawnX), floor(spawnY), false);
                    _engine->_world->mapAddEntity(e);
                    _lastActiveEntity = e;
                    onEntityAdded(e);
                    _engine->_state &= ~StudioEngineState_DisplayEntities;
                }
            }
                continue;
            case NG_KEY_E:
            case NG_KEY_ESCAPE:
            case NG_KEY_BACK_SPACE:
                if (e.isDown())
                {
                    SET_BIT(_engine->_state, StudioEngineState_DisplayEntities, false);
                }
                continue;
            default:
                continue;
        }
    }
    
    const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
    int numEntityIndices = static_cast<int>(entityDescriptors.size()) - 1;
    _rawSelectionIndex = clamp(_rawSelectionIndex + _selectionIndexDir, numEntityIndices, 0);
    _selectionIndex = clamp(_rawSelectionIndex, numEntityIndices, 0);
}

void StudioInputManager::handleLoadMapDialogInput()
{
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent& e = *(*i);
        switch (e.getKey())
        {
            case NG_KEY_ARROW_DOWN:
                _selectionIndexDir = e.isPressed() ? 0.25f : 0;
                continue;
            case NG_KEY_ARROW_UP:
                _selectionIndexDir = e.isPressed() ? -0.25f : 0;
                continue;
            case NG_KEY_CARRIAGE_RETURN:
            {
                if (e.isDown())
                {
                    std::vector<MapDef>& maps = EntityLayoutMapper::getInstance()->getMaps();
                    uint32_t map = maps[_selectionIndex].key;
                    _engine->_world->loadMap(map);
                    onMapLoaded();
                    _engine->_state &= ~StudioEngineState_DisplayLoadMapDialog;
                }
            }
                continue;
            case NG_KEY_ESCAPE:
            case NG_KEY_BACK_SPACE:
                _engine->_state &= e.isDown() ? ~StudioEngineState_DisplayLoadMapDialog : 0;
                continue;
            default:
                continue;
        }
    }
    
    _rawSelectionIndex = clamp(_rawSelectionIndex + _selectionIndexDir, 1, 0);
    _selectionIndex = clamp(_rawSelectionIndex, 1, 0);
}

bool layerSort(Entity* l, Entity* r)
{
    TextureRegion ltr = ASSETS->findTextureRegion(l->getTextureMapping(), l->getStateTime());
    TextureRegion rtr = ASSETS->findTextureRegion(r->getTextureMapping(), r->getStateTime());
    
    return (ltr._layer > rtr._layer);
}

void StudioInputManager::onMapLoaded()
{
    _activeEntity = NULL;
    _lastActiveEntity = NULL;
    _entities.clear();
    
    std::vector<Entity*>& dynamicEntities = _engine->_world->getDynamicEntities();
    std::vector<Entity*>& staticEntities = _engine->_world->getStaticEntities();
    std::vector<Entity*>& layers = _engine->_world->getLayers();
    
    size_t size = dynamicEntities.size();
    size += staticEntities.size();
    size += layers.size();
    
    _entities.reserve(size);
    
    _entities.insert(_entities.end(), dynamicEntities.begin(), dynamicEntities.end());
    _entities.insert(_entities.end(), staticEntities.begin(), staticEntities.end());
    _entities.insert(_entities.end(), layers.begin(), layers.end());
    
    std::sort(_entities.begin(), _entities.end(), layerSort);
}

void StudioInputManager::onEntityAdded(Entity* e)
{
    _entities.push_back(e);
    std::sort(_entities.begin(), _entities.end(), layerSort);
}

void StudioInputManager::onEntityRemoved(Entity* e)
{
    for (std::vector<Entity*>::iterator i = _entities.begin(); i != _entities.end(); )
    {
        Entity* ie = (*i);
        if (ie == e)
        {
            i = _entities.erase(i);
            return;
        }
        else
        {
            ++i;
        }
    }
}

Entity* StudioInputManager::getEntityAtPosition(float x, float y)
{
    if (_lastActiveEntity && entityExistsAtPosition(_lastActiveEntity, x, y))
    {
        return _lastActiveEntity;
    }
    
    for (std::vector<Entity*>::iterator i = _entities.begin(); i != _entities.end(); ++i)
    {
        Entity* e = (*i);
        TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        int layer = tr._layer;
        if (_engine->_state & (1 << (layer + StudioEngineState_LayerBitBegin)))
        {
            if (entityExistsAtPosition(e, x, y))
            {
                return e;
            }
        }
    }
    
    return NULL;
}

bool StudioInputManager::entityExistsAtPosition(Entity* e, float x, float y)
{
    float eX = e->getPosition().x;
    float eY = e->getPosition().y;
    float eW = e->getWidth();
    float eH = e->getHeight();
    NGRect entityBounds = NGRect(eX - eW / 2, eY - eH / 2, eW, eH);
    if (OverlapTester::isPointInNGRect(x, y, entityBounds))
    {
        return true;
    }
    
    return false;
}

void StudioInputManager::onInputProcessed()
{
    _liveInput.clear();
    _isTimeToProcessInput = false;
}

void StudioInputManager::updateCamera()
{
    int w = GM_CFG->_camWidth * _scrollValue;
    int h = GM_CFG->_camHeight * _scrollValue;
    float topPan = h * 0.95f;
    float bottomPan = h * 0.05f;
    float rightPan = w * 0.95f;
    float leftPan = w * 0.05f;
    
    if (_lastScrollValue != _scrollValue)
    {
        CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth * _lastScrollValue, GM_CFG->_camHeight * _lastScrollValue);
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        
        int dw = GM_CFG->_camWidth * _lastScrollValue;
        int dh = GM_CFG->_camHeight * _lastScrollValue;
        
        float xFactor = c.getX() / dw;
        float yFactor = c.getY() / dh;
        
        Vector2 center = _cursor;
        center.add(dw * xFactor, dh * yFactor);
        center.sub(w * xFactor, h * yFactor);
        
        _cursor = center;
        
        _rawScrollValue = _scrollValue;
        _lastScrollValue = _scrollValue;
        CURSOR_CONVERTER->setCamSize(GM_CFG->_camWidth * _scrollValue, GM_CFG->_camHeight * _scrollValue);
    }
    
    {
        /// Update Camera based on mouse being near edges
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        if ((_hasTouchedScreen && c.getY() > topPan) || _isPanningUp)
        {
            _cursor.add(0, h / GM_CFG->_camHeight / 2.0f);
            _activeEntityCursor.add(0, h / GM_CFG->_camHeight / 2.0f);
        }
        if ((_hasTouchedScreen && c.getY() < bottomPan) || _isPanningDown)
        {
            _cursor.sub(0, h / GM_CFG->_camHeight / 2.0f);
            _activeEntityCursor.sub(0, h / GM_CFG->_camHeight / 2.0f);
        }
        if ((_hasTouchedScreen && c.getX() > rightPan) || _isPanningRight)
        {
            _cursor.add(w / GM_CFG->_camWidth / 2.0f, 0);
            _activeEntityCursor.add(w / GM_CFG->_camWidth / 2.0f, 0);
        }
        if ((_hasTouchedScreen && c.getX() < leftPan) || _isPanningLeft)
        {
            _cursor.sub(w / GM_CFG->_camWidth / 2.0f, 0);
            _activeEntityCursor.sub(w / GM_CFG->_camWidth / 2.0f, 0);
        }
    }
    
    _engine->_renderer->update(_cursor.getX(), _cursor.getY(), w, h, _scrollValue);
}

void StudioInputManager::resetCamera()
{
    _rawScrollValue = 1;
    _scrollValue = 1;
    _lastScrollValue = 1;
    int w = GM_CFG->_camWidth * _scrollValue;
    int h = GM_CFG->_camHeight * _scrollValue;
    CURSOR_INPUT_MANAGER->resetScrollValue();
    CURSOR_CONVERTER->setCamSize(w, h);
    _cursor.set(0, 0);
}

StudioInputManager::StudioInputManager() :
_downCursor(),
_dragCursor(),
_deltaCursor(),
_cursor(),
_upCursor(),
_liveInput(),
_inputState(SIMS_NONE),
_isTimeToProcessInput(false),
_isControl(false),
_rawScrollValue(1),
_scrollValue(1),
_lastScrollValue(1),
_isPanningUp(false),
_isPanningDown(false),
_isPanningRight(false),
_isPanningLeft(false),
_selectionIndex(0),
_rawSelectionIndex(0),
_selectionIndexDir(0),
_activeEntity(NULL),
_lastActiveEntity(NULL),
_activeEntityCursor(),
_engine(NULL),
_hasTouchedScreen(false)
{
    resetCamera();
}
