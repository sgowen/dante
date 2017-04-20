//
//  TextureWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureWrapper__
#define __noctisgames__TextureWrapper__

#include <string>

struct GpuTextureDataWrapper;
struct GpuTextureWrapper;

class TextureWrapper
{
public:
    std::string name;
    GpuTextureDataWrapper* gpuTextureDataWrapper;
    GpuTextureWrapper* gpuTextureWrapper;
    bool repeatS;
    bool isLoadingData;
    
    TextureWrapper(std::string inName, bool in_repeatS = false);
};

#endif /* defined(__noctisgames__TextureWrapper__) */
