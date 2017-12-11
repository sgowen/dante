//
//  DirectXGeometryProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXGeometryProgram__
#define __noctisgames__DirectXGeometryProgram__

#include "framework/graphics/directx/DirectXProgram.h"

class DirectXGeometryProgram : public DirectXProgram
{
public:
    DirectXGeometryProgram(const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void mapVertices();
};

#endif /* defined(__noctisgames__DirectXGeometryProgram__) */
