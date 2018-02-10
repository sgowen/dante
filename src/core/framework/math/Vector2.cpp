//
//  Vector2.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/math/Vector2.h>

#include <framework/util/macros.h>

#include <framework/util/MathUtil.h>

const Vector2 Vector2::Zero(0.0f, 0.0f);

Vector2 operator+(const Vector2& inA, const Vector2& inB)
{
    return Vector2(inA._x + inB._x, inA._y + inB._y);
}

Vector2 operator-(const Vector2& inA, const Vector2& inB)
{
    return Vector2(inA._x - inB._x, inA._y - inB._y);
}

Vector2 operator*(float inScalar, const Vector2& inVec)
{
    return Vector2(inVec.getX() * inScalar, inVec.getY() * inScalar);
}

Vector2 operator*(const Vector2& inVec, float inScalar)
{
    return Vector2(inVec.getX() * inScalar, inVec.getY() * inScalar);
}

Vector2 operator*(const Vector2& inA, const Vector2& inB)
{
    return Vector2(inA._x * inB._x, inA._y * inB._y);
}

Vector2 lerp(Vector2& inA, Vector2& inB, float t)
{
    return Vector2(inA + t * (inB - inA));
}

Vector2::Vector2(float x, float y) : _x(x), _y(y)
{
    // Empty
}

Vector2::Vector2(const Vector2 &cSource)
{
    _x = cSource.getX();
    _y = cSource.getY();
}

Vector2 Vector2::set(const Vector2 &other)
{
    _x = other.getX();
    _y = other.getY();
    
    return *this;
}

Vector2 Vector2::set(float x, float y)
{
    _x = x;
    _y = y;
    
    return *this;
}

Vector2 Vector2::setX(float x)
{
	_x = x;
    
    return *this;
}

Vector2 Vector2::setY(float y)
{
	_y = y;
    
    return *this;
}

Vector2 Vector2::cpy()
{
	return Vector2(_x, _y);
}

void Vector2::operator+=(const Vector2 &other)
{
    add(other.getX(), other.getY());
}

Vector2 Vector2::add(float x, float y)
{
    _x += x;
    _y += y;
    
    return *this;
}

void Vector2::operator-=(const Vector2 &other)
{
    sub(other.getX(), other.getY());
}

Vector2 Vector2::sub(float x, float y)
{
    _x -= x;
    _y -= y;
    
    return *this;
}

Vector2 Vector2::operator*=(float scalar)
{
    return mul(scalar);
}

Vector2 Vector2::mul(float scalar)
{
    _x *= scalar;
    _y *= scalar;
    
    return *this;
}

Vector2 Vector2::operator/=(float scalar)
{
    return div(scalar);
}

Vector2 Vector2::div(float scalar)
{
    _x /= scalar;
    _y /= scalar;
    
    return *this;
}

float Vector2::len()
{
    return sqrtf(_x * _x + _y * _y);
}

float Vector2::lenSquared()
{
    return _x * _x + _y * _y;
}

Vector2 Vector2::nor()
{
    float l = len();
    
    if (l != 0)
    {
        _x /= l;
        _y /= l;
    }
    
    return *this;
}

float Vector2::angle()
{
    float radians = atan2f(_y, _x);
    float angle = RADIANS_TO_DEGREES(radians);
    
    if (angle < 0)
    {
        angle += 360;
    }
    
    return angle;
}

Vector2 Vector2::rotate(float angle)
{
    float rad = DEGREES_TO_RADIANS(angle);
    float cos = cosf(rad);
    float sin = sinf(rad);
    
    float newX = _x * cos - _y * sin;
    float newY = _x * sin + _y * cos;
    
    _x = newX;
    _y = newY;
    
    return *this;
}

float Vector2::dot(const Vector2 &other)
{
    return (_x * other._x + _y * other._y);
}

float Vector2::dist(const Vector2 &other) const
{
    return dist(other.getX(), other.getY());
}

float Vector2::dist(float x, float y) const
{
    return sqrtf(distSquared(x, y));
}

float Vector2::distSquared(const Vector2 &other) const
{
    return distSquared(other.getX(), other.getY());
}

float Vector2::distSquared(float x, float y) const
{
    float distX = _x - x;
    float distY = _y - y;
    
    return distX * distX + distY * distY;
}

bool Vector2::isEqualTo(const Vector2& other)
{
    return areFloatsPracticallyEqual(_x, other._x) && areFloatsPracticallyEqual(_y, other._y);
}

float Vector2::getX() const
{
    return _x;
}

float Vector2::getY() const
{
    return _y;
}
