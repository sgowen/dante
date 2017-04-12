//
//  Direct3DTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DTextureProgram__
#define __noctisgames__Direct3DTextureProgram__

#include "Direct3DProgram.h"

class Direct3DTextureProgram : public Direct3DProgram
{
public:
    Direct3DTextureProgram(_In_z_ const wchar_t* vertexShaderName, _In_z_ const wchar_t* pixelShaderName);
    
    virtual void mapVertices();
};

#endif /* defined(__noctisgames__Direct3DTextureProgram__) */
