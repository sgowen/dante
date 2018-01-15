//
//  Vector3.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/math/Vector3.h"

#include "framework/util/macros.h"

#include "framework/math/MathUtil.h"

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);

Vector3::Vector3(float x, float y, float z) : _x(x), _y(y), _z(z)
{
    // Empty
}

Vector3::Vector3(const Vector3 &cSource)
{
    _x = cSource.getX();
    _y = cSource.getY();
    _z = cSource.getZ();
}

Vector3 Vector3::set(float x, float y, float z)
{
    _x = x;
    _y = y;
    _z = z;
    
    return *this;
}

Vector3 Vector3::setX(float x)
{
    _x = x;
    
    return *this;
}

Vector3 Vector3::setY(float y)
{
    _y = y;
    
    return *this;
}

Vector3 Vector3::setZ(float z)
{
    _z = z;
    
    return *this;
}

float Vector3::getX() const
{
    return _x;
}

float Vector3::getY() const
{
    return _y;
}

float Vector3::getZ() const
{
    return _z;
}
