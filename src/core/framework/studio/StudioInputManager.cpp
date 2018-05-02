//
//  StudioInputManager.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/studio/StudioInputManager.h>

#include <framework/entity/EntityIDManager.h>
#include <framework/studio/StudioEngine.h>
#include <framework/entity/Entity.h>

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
#include <framework/studio/StudioRenderer.h>
#include <framework/studio/World.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityMapper.h>
#include <framework/math/OverlapTester.h>
#include <framework/math/NGRect.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/file/portable/Assets.h>
#include <framework/util/macros.h>
#include <framework/util/InstanceManager.h>
#include <framework/util/Config.h>

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
    if (state & SES_TextInput)
    {
        handleTextInput();
    }
    else if (state & SES_DisplayLoadMapDialog)
    {
        handleLoadMapDialogInput();
    }
    else if (state & SES_DisplayEntities)
    {
        handleEntitiesInput();
    }
    else
    {
        handleDefaultInput();
        updateCamera();
    }
    
    int w = FW_CFG->_camWidth * _scrollValue;
    int h = FW_CFG->_camHeight * _scrollValue;
    _engine->_renderer->update(_cursor.getX(), _cursor.getY(), w, h, _scrollValue);
}

std::string& StudioInputManager::getLiveInput()
{
    return _liveInput;
}

void StudioInputManager::handleDefaultInput()
{
    _rawScrollValue = clamp(_rawScrollValue + CURSOR_INPUT_MANAGER->getScrollWheelValue(), 1, 16);
    CURSOR_INPUT_MANAGER->resetScrollValue();
    _scrollValue = clamp(_rawScrollValue, 1, 16);
    CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth * _scrollValue, FW_CFG->_camHeight * _scrollValue);
    
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
                        _engine->_world->removeEntity(_lastActiveEntity);
                        
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
                    float x = clamp(position.x, width / 2, FLT_MAX);
                    float y = clamp(position.y, height / 2, FLT_MAX);
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
                _engine->_state ^= e.isDown() ? SES_Layer0 : 0;
                continue;
            case NG_KEY_ONE:
                _engine->_state ^= e.isDown() ? SES_Layer1 : 0;
                continue;
            case NG_KEY_TWO:
                _engine->_state ^= e.isDown() ? SES_Layer2 : 0;
                continue;
            case NG_KEY_THREE:
                _engine->_state ^= e.isDown() ? SES_Layer3 : 0;
                continue;
            case NG_KEY_FOUR:
                _engine->_state ^= e.isDown() ? SES_Layer4 : 0;
                continue;
            case NG_KEY_FIVE:
                _engine->_state ^= e.isDown() ? SES_Layer5 : 0;
                continue;
            case NG_KEY_SIX:
                _engine->_state ^= e.isDown() ? SES_Layer6 : 0;
                continue;
            case NG_KEY_SEVEN:
                _engine->_state ^= e.isDown() ? SES_Layer7 : 0;
                continue;
            case NG_KEY_EIGHT:
                _engine->_state ^= e.isDown() ? SES_Layer8 : 0;
                continue;
            case NG_KEY_NINE:
                _engine->_state ^= e.isDown() ? SES_Layer9 : 0;
                continue;
            case NG_KEY_D:
                _engine->_state ^= e.isDown() ? SES_DisplayTypes : 0;
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
                _engine->_state ^= e.isDown() ? SES_DisplayControls : 0;
                continue;
            case NG_KEY_A:
                _engine->_state ^= e.isDown() ? SES_DisplayAssets : 0;
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
                        _engine->_state |= SES_DisplayEntities;
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
                _engine->_state |= e.isDown() ? SES_DisplayNewMapDialog : 0;
                if (e.isDown())
                {
                    _engine->_renderer->displayToast("New Map not yet implemented...");
                }
                continue;
            case NG_KEY_L:
                _engine->_state |= e.isDown() ? SES_DisplayLoadMapDialog : 0;
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
                        _engine->_world->saveMapAs(s_testMap);
                        _engine->_testFunc(_engine->_engine, s_testMap);
                        return;
                    }
                }
            }
                continue;
            case NG_KEY_S:
                if (e.isDown())
                {
                    if (!_engine->_world->isMapLoaded())
                    {
                        _engine->_renderer->displayToast("Load a Map first!");
                        return;
                    }
                    
                    if (_engine->_world->getDynamicEntities().size() > MAX_NUM_DYNAMIC_ENTITIES)
                    {
                        _engine->_renderer->displayToast(StringUtil::format("Cannot have more than %d dynamic entities!", MAX_NUM_DYNAMIC_ENTITIES));
                    }
                    else
                    {
                        if (_isControl)
                        {
                            _engine->_state |= e.isDown() ? SES_DisplaySaveMapAsDialog : 0;
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
                _engine->_state ^= e.isDown() ? SES_DisplayParallax : 0;
                continue;
            case NG_KEY_B:
                _engine->_state ^= e.isDown() ? SES_DisplayBox2D : 0;
                continue;
            case NG_KEY_G:
                _engine->_state ^= e.isDown() ? SES_DisplayGrid : 0;
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
        KeyboardEvent* e = (*i);
        bool isSupported = true;
        if (_engine->_textInputType == SETIT_Integer)
        {
            isSupported = isKeyInteger(e->getKey());
        }
        
        if (isKeySupported(e->getKey()))
        {
            if (e->isDown()
                && !e->isHeld())
            {
                if (e->getKey() == NG_KEY_CARRIAGE_RETURN)
                {
                    processInput();
                    return;
                }
                else if (e->getKey() == NG_KEY_ESCAPE)
                {
                    return;
                }
                else if (e->getKey() == NG_KEY_BACK_SPACE
                         || e->getKey() == NG_KEY_DELETE)
                {
                    std::string s = ss.str();
                    _liveInput += s;
                    if (_liveInput.end() > _liveInput.begin())
                    {
                        _liveInput.erase(_liveInput.end() - 1, _liveInput.end());
                    }
                    return;
                }
                else if (isSupported)
                {
                    char key = charForKey(e->getKey());
                    ss << StringUtil::format("%c", key);
                }
            }
            else if (e->isUp())
            {
                if (e->getKey() == NG_KEY_ESCAPE)
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

void StudioInputManager::handleEntitiesInput()
{
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent& e = *(*i);
        switch (e.getKey())
        {
            case NG_KEY_ZERO:
                _engine->_state ^= e.isDown() ? SES_Layer0 : 0;
                continue;
            case NG_KEY_ONE:
                _engine->_state ^= e.isDown() ? SES_Layer1 : 0;
                continue;
            case NG_KEY_TWO:
                _engine->_state ^= e.isDown() ? SES_Layer2 : 0;
                continue;
            case NG_KEY_THREE:
                _engine->_state ^= e.isDown() ? SES_Layer3 : 0;
                continue;
            case NG_KEY_FOUR:
                _engine->_state ^= e.isDown() ? SES_Layer4 : 0;
                continue;
            case NG_KEY_FIVE:
                _engine->_state ^= e.isDown() ? SES_Layer5 : 0;
                continue;
            case NG_KEY_SIX:
                _engine->_state ^= e.isDown() ? SES_Layer6 : 0;
                continue;
            case NG_KEY_SEVEN:
                _engine->_state ^= e.isDown() ? SES_Layer7 : 0;
                continue;
            case NG_KEY_EIGHT:
                _engine->_state ^= e.isDown() ? SES_Layer8 : 0;
                continue;
            case NG_KEY_NINE:
                _engine->_state ^= e.isDown() ? SES_Layer9 : 0;
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
                    else if (entityDef->type == 'WATR')
                    {
                        _engine->_state |= SES_TextInput;
                        _engine->_textInputField = SETIF_WaterDepth;
                        _engine->_textInputType = SETIT_Integer;
                        return;
                    }
                    
                    addEntity(entityDef);
                }
            }
                continue;
            case NG_KEY_E:
            case NG_KEY_ESCAPE:
            case NG_KEY_BACK_SPACE:
                if (e.isDown())
                {
                    SET_BIT(_engine->_state, SES_DisplayEntities, false);
                }
                continue;
            default:
                continue;
        }
    }
    
    const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
    int numEntityIndices = static_cast<int>(entityDescriptors.size()) - 1;
    _rawSelectionIndex = clamp(_rawSelectionIndex + _selectionIndexDir, 0, numEntityIndices);
    _selectionIndex = clamp(_rawSelectionIndex, 0, numEntityIndices);
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
                    _engine->_state &= ~SES_DisplayLoadMapDialog;
                }
            }
                continue;
            case NG_KEY_ESCAPE:
            case NG_KEY_BACK_SPACE:
                _engine->_state &= e.isDown() ? ~SES_DisplayLoadMapDialog : 0;
                continue;
            default:
                continue;
        }
    }
    
    _rawSelectionIndex = clamp(_rawSelectionIndex + _selectionIndexDir, 0, 1);
    _selectionIndex = clamp(_rawSelectionIndex, 0, 1);
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
    std::vector<Entity*>& waterBodies = _engine->_world->getWaterBodies();
    std::vector<Entity*>& staticEntities = _engine->_world->getStaticEntities();
    std::vector<Entity*>& layers = _engine->_world->getLayers();
    
    size_t size = dynamicEntities.size();
    size += waterBodies.size();
    size += staticEntities.size();
    size += layers.size();
    
    _entities.reserve(size);
    
    _entities.insert(_entities.end(), dynamicEntities.begin(), dynamicEntities.end());
    _entities.insert(_entities.end(), waterBodies.begin(), waterBodies.end());
    _entities.insert(_entities.end(), staticEntities.begin(), staticEntities.end());
    _entities.insert(_entities.end(), layers.begin(), layers.end());
    
    std::sort(_entities.begin(), _entities.end(), layerSort);
    
    _engine->_renderer->onMapLoaded();
}

void StudioInputManager::onEntityAdded(Entity* e)
{
    _entities.push_back(e);
    std::sort(_entities.begin(), _entities.end(), layerSort);
}

void StudioInputManager::onEntityRemoved(Entity* e)
{
    if (e->getEntityDef().type == 'WATR')
    {
        _engine->_renderer->onWaterRemoved(e);
    }
    
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
        if (_engine->_state & (1 << (layer + SES_LayerBitBegin)))
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

Entity* StudioInputManager::addEntity(EntityDef* entityDef, int width, int height)
{
    float spawnX = clamp(FW_CFG->_camWidth * _scrollValue / 2 + _cursor.getX(), entityDef->width / 2.0f, FLT_MAX);
    float spawnY = clamp(FW_CFG->_camHeight * _scrollValue / 2 + _cursor.getY(), entityDef->height / 2.0f, FLT_MAX);
    EntityInstanceDef eid(_entityIDManager->getNextStaticEntityID(), entityDef->type, floor(spawnX), floor(spawnY), width, height);
    Entity* e = EntityMapper::getInstance()->createEntityFromDef(entityDef, &eid, false);
    _engine->_world->addEntity(e);
    _lastActiveEntity = e;
    onEntityAdded(e);
    _engine->_state &= ~SES_DisplayEntities;
    
    return e;
}

void StudioInputManager::processInput()
{
    bool clearInput = false;
    bool exitTextInputState = false;
    if (_engine->_textInputField == SETIF_WaterDepth)
    {
        _waterDepth = StringUtil::stringToNumber<int>(_liveInput);
        if (_waterDepth > 0 && _waterDepth % 2 == 0)
        {
            _engine->_textInputField = SETIF_WaterWidth;
            clearInput = true;
        }
        else
        {
            _engine->_renderer->displayToast("Water Depth must be at least 2 tiles (2x16=32pix)");
        }
    }
    else if (_engine->_textInputField == SETIF_WaterWidth)
    {
        _waterWidth = StringUtil::stringToNumber<int>(_liveInput);
        if (_waterWidth > 0 && _waterWidth % 2 == 0)
        {
            Entity* e = addEntity(EntityMapper::getInstance()->getEntityDef('WATR'), _waterWidth, _waterDepth);
            _engine->_renderer->onWaterAdded(e);
            _waterWidth = 0;
            _waterDepth = 0;
            clearInput = true;
            exitTextInputState = true;
        }
        else
        {
            _engine->_renderer->displayToast("Water Width must be at least 2 tiles (2x16=32pix)");
        }
    }
    
    if (clearInput)
    {
        _liveInput.clear();
    }
    
    if (exitTextInputState)
    {
        SET_BIT(_engine->_state, SES_TextInput, false);
        _engine->_textInputField = 0;
        _engine->_textInputType = 0;
    }
}

void StudioInputManager::updateCamera()
{
    int w = FW_CFG->_camWidth * _scrollValue;
    int h = FW_CFG->_camHeight * _scrollValue;
    float topPan = h * 0.95f;
    float bottomPan = h * 0.05f;
    float rightPan = w * 0.95f;
    float leftPan = w * 0.05f;
    
    if (_lastScrollValue != _scrollValue)
    {
        CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth * _lastScrollValue, FW_CFG->_camHeight * _lastScrollValue);
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        
        int dw = FW_CFG->_camWidth * _lastScrollValue;
        int dh = FW_CFG->_camHeight * _lastScrollValue;
        
        float xFactor = c.getX() / dw;
        float yFactor = c.getY() / dh;
        
        Vector2 center = _cursor;
        center.add(dw * xFactor, dh * yFactor);
        center.sub(w * xFactor, h * yFactor);
        
        _cursor = center;
        
        _rawScrollValue = _scrollValue;
        _lastScrollValue = _scrollValue;
        CURSOR_CONVERTER->setCamSize(FW_CFG->_camWidth * _scrollValue, FW_CFG->_camHeight * _scrollValue);
    }
    
    {
        /// Update Camera based on mouse being near edges
        Vector2& rc = CURSOR_INPUT_MANAGER->getCursorPosition();
        Vector2 c = CURSOR_CONVERTER->convert(rc);
        if ((_hasTouchedScreen && c.getY() > topPan) || _isPanningUp)
        {
            _cursor.add(0, h / FW_CFG->_camHeight / 2.0f);
            _activeEntityCursor.add(0, h / FW_CFG->_camHeight / 2.0f);
        }
        if ((_hasTouchedScreen && c.getY() < bottomPan) || _isPanningDown)
        {
            _cursor.sub(0, h / FW_CFG->_camHeight / 2.0f);
            _activeEntityCursor.sub(0, h / FW_CFG->_camHeight / 2.0f);
        }
        if ((_hasTouchedScreen && c.getX() > rightPan) || _isPanningRight)
        {
            _cursor.add(w / FW_CFG->_camWidth / 2.0f, 0);
            _activeEntityCursor.add(w / FW_CFG->_camWidth / 2.0f, 0);
        }
        if ((_hasTouchedScreen && c.getX() < leftPan) || _isPanningLeft)
        {
            _cursor.sub(w / FW_CFG->_camWidth / 2.0f, 0);
            _activeEntityCursor.sub(w / FW_CFG->_camWidth / 2.0f, 0);
        }
    }
}

void StudioInputManager::resetCamera()
{
    int w = FW_CFG->_camWidth * _scrollValue;
    int h = FW_CFG->_camHeight * _scrollValue;
    CURSOR_INPUT_MANAGER->resetScrollValue();
    CURSOR_CONVERTER->setCamSize(w, h);
    _cursor.set(0, 0);
}

StudioInputManager::StudioInputManager() :
_entityIDManager(static_cast<EntityIDManager*>(INSTANCE_MANAGER->get(INSTANCE_ENTITY_ID_MANAGER_STUDIO))),
_downCursor(),
_dragCursor(),
_deltaCursor(),
_cursor(),
_upCursor(),
_liveInput(),
_inputState(SIMS_NONE),
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
_engine(StudioEngine::getInstance()),
_hasTouchedScreen(false),
_waterWidth(0),
_waterDepth(0)
{
    resetCamera();
}
