//
//  GpuProgramWrapperFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuProgramWrapperFactory__
#define __noctisgames__GpuProgramWrapperFactory__

class GpuProgramWrapper;
class RendererHelper;

#define GPU_PROGRAM_WRAPPER_FACTORY (GpuProgramWrapperFactory::getInstance())

class GpuProgramWrapperFactory
{
public:
	static GpuProgramWrapperFactory* getInstance();
    
    GpuProgramWrapper* createTextureGpuProgramWrapper(RendererHelper* inRendererHelper);
    
    GpuProgramWrapper* createColorGpuProgramWrapper(RendererHelper* inRendererHelper);
    
    GpuProgramWrapper* createFramebufferToScreenGpuProgramWrapper(RendererHelper* inRendererHelper);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    GpuProgramWrapperFactory();
    GpuProgramWrapperFactory(const GpuProgramWrapperFactory&);
    GpuProgramWrapperFactory& operator=(const GpuProgramWrapperFactory&);
};

#endif /* defined(__noctisgames__GpuProgramWrapperFactory__) */
