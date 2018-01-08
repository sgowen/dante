//
//  FlagUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/6/13.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FlagUtil__
#define __noctisgames__FlagUtil__

template <typename T>
inline bool isFlagSet(T value, int flag)
{
    return (value & flag) == flag;
}

template <typename T>
inline void setFlag(T& value, int flag)
{
    value |= flag;
}

template <typename T>
inline void removeFlag(T& value, int flag)
{
    value &= ~flag;
}

#endif /* defined(__noctisgames__FlagUtil__) */
