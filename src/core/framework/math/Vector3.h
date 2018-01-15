//
//  Vector3.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Vector3__
#define __noctisgames__Vector3__

class Vector3
{
public:
    static const Vector3 Zero;
    
    Vector3(float x = 0, float y = 0, float z = 0);
    Vector3(const Vector3 &cSource);
    
    Vector3 set(float x, float y, float z);
    Vector3 setX(float x);
    Vector3 setY(float y);
    Vector3 setZ(float z);
    
    float getX() const;
    float getY() const;
    float getZ() const;
    
private:
    float _x;
    float _y;
    float _z;
};

#endif /* defined(__noctisgames__Vector3__) */
