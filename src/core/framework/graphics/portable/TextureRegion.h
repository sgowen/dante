//
//  TextureRegion.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureRegion__
#define __noctisgames__TextureRegion__

#include <string>

class TextureRegion
{
public:
    double _u1, _v1, _u2, _v2;
    double _x, _y, _regionWidth, _regionHeight, _textureWidth, _textureHeight;
    int _layer;
    
    TextureRegion(std::string textureName, int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight, int layer);
    
    std::string& getTextureName();
    std::string& getNormalMapName();
    
private:
    std::string _textureName;
    std::string _normalMapName;
};

#endif /* defined(__noctisgames__TextureRegion__) */
