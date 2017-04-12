//
//  Direct3DGeometryGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DGeometryGpuProgramWrapper__
#define __noctisgames__Direct3DGeometryGpuProgramWrapper__

#include "GpuProgramWrapper.h"

class Direct3DProgram;

class Direct3DGeometryGpuProgramWrapper : public GpuProgramWrapper
{
public:
	Direct3DGeometryGpuProgramWrapper();
    
    virtual ~Direct3DGeometryGpuProgramWrapper();
    
	virtual void bind();

	virtual void unbind();

private:
    Direct3DProgram* m_program;
};

#endif /* defined(__noctisgames__Direct3DGeometryGpuProgramWrapper__) */
