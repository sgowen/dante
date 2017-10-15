//
//  nullptr_emulation.h
//  dante
//
//  Created by Stephen Gowen on 10/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__nullptr_emulation__
#define __noctisgames__nullptr_emulation__

const // It is a const object...
class nullptr_t
{
public:
    template<class T>
    inline operator T*() const // convertible to any type of null non-member pointer...
    { return 0; }
    
    template<class C, class T>
    inline operator T C::*() const   // or any type of null member pointer...
    { return 0; }
    
private:
    void operator&() const;  // Can't take address of nullptr
    
} nullptr = {};

#endif /* defined(__noctisgames__nullptr_emulation__) */
