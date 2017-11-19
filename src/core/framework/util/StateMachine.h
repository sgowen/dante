//
//  StateMachine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StateMachine__
#define __noctisgames__StateMachine__

#include <cassert>

#include "framework/util/State.h"

template <class entity_type, class state_type>
class StateMachine
{
public:
    
    StateMachine(entity_type* owner) : _owner(owner), _currentState(nullptr), _previousState(nullptr), _globalState(nullptr)
    {
        // Empty
    }
    
    virtual ~StateMachine()
    {
        // Empty
    }
    
    void setCurrentState(state_type* state)
    {
        _currentState = state;
    }
    
    void setGlobalState(state_type* state)
    {
        _globalState = state;
    }
    
    void setPreviousState(state_type* state)
    {
        _previousState = state;
    }
    
    void execute() const
    {
        if (_globalState)
        {
            _globalState->execute(_owner);
        }
        
        if (_currentState)
        {
            _currentState->execute(_owner);
        }
    }
    
    void changeState(state_type* newState)
    {
        assert(newState && "<StateMachine::ChangeState>:trying to assign null state to current");
        
        _previousState = _currentState;
        
        _currentState->exit(_owner);
        
        _currentState = newState;
        
        _currentState->enter(_owner);
    }
    
    void revertToPreviousState()
    {
        changeState(_previousState);
    }
    
    bool isInState(const state_type* st)const
    {
        return _currentState == st;
    }
    
    state_type* getCurrentState() const
    {
        return _currentState;
    }
    
    state_type* getGlobalState() const
    {
        return _globalState;
    }
    
    state_type* getPreviousState() const
    {
        return _previousState;
    }
    
private:
    entity_type* _owner;
    
    state_type* _currentState;
    state_type* _previousState;
    state_type* _globalState;
};

#endif /* defined(__noctisgames__StateMachine__) */
