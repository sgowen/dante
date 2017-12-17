//
//  Color.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Color__
#define __noctisgames__Color__

class Color
{
public:
    static Color WHITE;
    static Color BLACK;
    static Color RED;
    static Color DARK_RED;
    
    float red;
    float green;
    float blue;
    float alpha;

    Color(float redIn, float greenIn, float blueIn, float alphaIn);
    
    Color();
};

#endif /* defined(__noctisgames__Color__) */
