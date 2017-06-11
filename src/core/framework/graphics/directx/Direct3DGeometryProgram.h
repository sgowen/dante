//
//  Direct3DGeometryProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DGeometryProgram__
#define __noctisgames__Direct3DGeometryProgram__

#include "Direct3DProgram.h"

class Direct3DGeometryProgram : public Direct3DProgram
{
public:
    Direct3DGeometryProgram(const char* vertexShaderName, const char* pixelShaderName);
    
    virtual void mapVertices();
};

#endif /* defined(__noctisgames__Direct3DGeometryProgram__) */
