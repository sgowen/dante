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
    
    // Set overwrite to true when transition away from a Loading Screen
    void changeState(state_type* newState, bool overwrite = false)
    {
        assert(newState);
        
        exit();
        
        if (overwrite)
        {
            _states.pop();
        }
        
        _states.push(newState);
        
        enter();
    }
    
    void revertToPreviousState()
    {
        exit();
        
        _states.pop();
        
        enter();
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
    
    void exit()
    {
        assert(getCurrentState());
        
        getCurrentState()->exit(_owner);
    }
    
    void enter()
    {
        assert(getCurrentState());
        
        getCurrentState()->enter(_owner);
    }
};

#endif /* defined(__noctisgames__StateMachine__) */
