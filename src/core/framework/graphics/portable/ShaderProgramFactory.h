//
//  ShaderProgramFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgramFactory__
#define __noctisgames__ShaderProgramFactory__

class ShaderProgram;
class RendererHelper;

#define GPU_PROGRAM_WRAPPER_FACTORY (ShaderProgramFactory::getInstance())

class ShaderProgramFactory
{
public:
	static ShaderProgramFactory* getInstance();
    
    ShaderProgram* createTextureShaderProgram(RendererHelper* inRendererHelper);
    
    ShaderProgram* createColorShaderProgram(RendererHelper* inRendererHelper);
    
    ShaderProgram* createFramebufferToScreenShaderProgram(RendererHelper* inRendererHelper);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    ShaderProgramFactory();
    ShaderProgramFactory(const ShaderProgramFactory&);
    ShaderProgramFactory& operator=(const ShaderProgramFactory&);
};

#endif /* defined(__noctisgames__ShaderProgramFactory__) */
