//
//  GpuProgramFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuProgramFactory__
#define __noctisgames__GpuProgramFactory__

class GpuProgram;
class RendererHelper;

#define GPU_PROGRAM_WRAPPER_FACTORY (GpuProgramFactory::getInstance())

class GpuProgramFactory
{
public:
	static GpuProgramFactory* getInstance();
    
    GpuProgram* createTextureGpuProgram(RendererHelper* inRendererHelper);
    
    GpuProgram* createColorGpuProgram(RendererHelper* inRendererHelper);
    
    GpuProgram* createFramebufferToScreenGpuProgram(RendererHelper* inRendererHelper);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    GpuProgramFactory();
    GpuProgramFactory(const GpuProgramFactory&);
    GpuProgramFactory& operator=(const GpuProgramFactory&);
};

#endif /* defined(__noctisgames__GpuProgramFactory__) */
