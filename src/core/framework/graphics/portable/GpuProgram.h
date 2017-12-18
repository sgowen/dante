//
//  GpuProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuProgram__
#define __noctisgames__GpuProgram__

#include <cstddef>

class GpuProgram
{
public:
    GpuProgram();
    
    virtual ~GpuProgram();
    
    virtual void bind(void* data = NULL) = 0;
    
    virtual void unbind() = 0;
};

#endif /* defined(__noctisgames__GpuProgram__) */
