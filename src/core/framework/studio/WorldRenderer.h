//
//  WorldRenderer.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/1/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef ___noctisgames___WorldRenderer__
#define ___noctisgames___WorldRenderer__

#include <vector>

class RendererHelper;

struct WorldRendererConfig
{
    bool parallax;
};

class WorldRenderer
{
public:
    WorldRenderer(RendererHelper* rendererHelper);
    ~WorldRenderer();
    
    void config(WorldRendererConfig config);
    
private:
    RendererHelper* _rendererHelper;
    WorldRendererConfig _config;
};

#endif /* defined(___noctisgames___WorldRenderer__) */
