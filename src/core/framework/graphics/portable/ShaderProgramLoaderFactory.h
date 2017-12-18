//
//  ShaderProgramLoaderFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgramLoaderFactory__
#define __noctisgames__ShaderProgramLoaderFactory__

class ShaderProgramLoader;

#define SHADER_PROGRAM_LOADER_FACTORY (ShaderProgramLoaderFactory::getInstance())

class ShaderProgramLoaderFactory
{
public:
	static ShaderProgramLoaderFactory* getInstance();
    
    ShaderProgramLoader* createShaderProgramLoader();

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    ShaderProgramLoaderFactory();
    ShaderProgramLoaderFactory(const ShaderProgramLoaderFactory&);
    ShaderProgramLoaderFactory& operator=(const ShaderProgramLoaderFactory&);
};

#endif /* defined(__noctisgames__ShaderProgramLoaderFactory__) */
