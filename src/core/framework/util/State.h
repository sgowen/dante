//
//  State.h
//  noctisgames
//
//  Created by Stephen Gowen on 8/28/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__State__
#define __noctisgames__State__

template <class object_type>
class State
{
public:
    State()
    {
        // Empty
    }
    
    virtual ~State()
    {
        // Empty
    }
    
    virtual void enter(object_type*) = 0;
    virtual void update(object_type*) = 0;
    virtual void exit(object_type*) = 0;
};

#endif /* defined(__noctisgames__State__) */
