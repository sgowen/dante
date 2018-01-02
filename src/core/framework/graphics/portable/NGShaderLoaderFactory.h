//
//  NGShaderLoaderFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShaderLoaderFactory__
#define __noctisgames__NGShaderLoaderFactory__

class NGShaderLoader;

#define SHADER_PROGRAM_LOADER_FACTORY (NGShaderLoaderFactory::getInstance())

class NGShaderLoaderFactory
{
public:
	static NGShaderLoaderFactory* getInstance();
    
    NGShaderLoader* createNGShaderLoader();

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGShaderLoaderFactory();
    NGShaderLoaderFactory(const NGShaderLoaderFactory&);
    NGShaderLoaderFactory& operator=(const NGShaderLoaderFactory&);
};

#endif /* defined(__noctisgames__NGShaderLoaderFactory__) */
