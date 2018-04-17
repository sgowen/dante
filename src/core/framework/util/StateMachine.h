//
//  StateMachine.h
//  noctisgames
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StateMachine__
#define __noctisgames__StateMachine__

#include <framework/util/State.h>

#include <stack>
#include <assert.h>

template <class object_type, class state_type>
class StateMachine
{
public:
    StateMachine(object_type* owner) : _owner(owner), _globalState(NULL)
    {
        // Empty
    }
    
    virtual ~StateMachine()
    {
        // Empty
    }
    
    void update() const
    {
        if (_globalState)
        {
            _globalState->update(_owner);
        }
        
        if (_states.size() > 0)
        {
            _states.top()->update(_owner);
        }
    }
    
    void changeState(state_type* newState)
    {
        assert(newState);
        
        if (getCurrentState())
        {
            getCurrentState()->exit(_owner);
        }
        
        _states.push(newState);
        
        assert(getCurrentState());
        getCurrentState()->enter(_owner);
    }
    
    void revertToPreviousState()
    {
        if (getCurrentState())
        {
            getCurrentState()->exit(_owner);
        }
        
        _states.pop();
        
        assert(getCurrentState());
        getCurrentState()->enter(_owner);
    }
    
    void setCurrentState(state_type* state)
    {
        while (!_states.empty())
        {
            _states.pop();
        }
        
        _states.push(state);
    }
    
    state_type* getCurrentState() const
    {
        return _states.size() > 0 ? _states.top() : NULL;
    }
    
    void setGlobalState(state_type* state)
    {
        _globalState = state;
    }
    
    state_type* getGlobalState() const
    {
        return _globalState;
    }
    
private:
    object_type* _owner;
    
    state_type* _globalState;
    std::stack<state_type*> _states;
};

#endif /* defined(__noctisgames__StateMachine__) */
