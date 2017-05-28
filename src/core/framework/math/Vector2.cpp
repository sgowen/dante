//
//  Vector2.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Vector2.h"

#include "macros.h"

#include <math.h>

const Vector2 Vector2::Zero(0.0f, 0.0f);

Vector2 operator+(const Vector2& inA, const Vector2& inB)
{
    return Vector2(inA.m_fX + inB.m_fX, inA.m_fY + inB.m_fY);
}

Vector2 operator-(const Vector2& inA, const Vector2& inB)
{
    return Vector2(inA.m_fX - inB.m_fX, inA.m_fY - inB.m_fY);
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
    return Vector2(inA.m_fX * inB.m_fX, inA.m_fY * inB.m_fY);
}

Vector2 lerp(Vector2& inA, Vector2& inB, float t)
{
    return Vector2(inA + t * (inB - inA));
}

Vector2::Vector2(float x, float y) : m_fX(x), m_fY(y)
{
    // Empty
}

Vector2::Vector2(const Vector2 &cSource)
{
    m_fX = cSource.getX();
    m_fY = cSource.getY();
}

Vector2 Vector2::set(const Vector2 &other)
{
    m_fX = other.getX();
    m_fY = other.getY();
    
    return *this;
}

Vector2 Vector2::set(float x, float y)
{
    m_fX = x;
    m_fY = y;
    
    return *this;
}

Vector2 Vector2::setX(float x)
{
	m_fX = x;
    
    return *this;
}

Vector2 Vector2::setY(float y)
{
	m_fY = y;
    
    return *this;
}

Vector2 Vector2::cpy()
{
	return Vector2(m_fX, m_fY);
}

void Vector2::operator+=(const Vector2 &other)
{
    add(other.getX(), other.getY());
}

Vector2 Vector2::add(float x, float y)
{
    m_fX += x;
    m_fY += y;
    
    return *this;
}

void Vector2::operator-=(const Vector2 &other)
{
    sub(other.getX(), other.getY());
}

Vector2 Vector2::sub(float x, float y)
{
    m_fX -= x;
    m_fY -= y;
    
    return *this;
}

Vector2 Vector2::operator*=(float scalar)
{
    return mul(scalar);
}

Vector2 Vector2::mul(float scalar)
{
    m_fX *= scalar;
    m_fY *= scalar;
    
    return *this;
}

float Vector2::len()
{
    return sqrtf(m_fX * m_fX + m_fY * m_fY);
}

float Vector2::lenSquared()
{
    return m_fX * m_fX + m_fY * m_fY;
}

Vector2 Vector2::nor()
{
    float l = len();
    
    if (l != 0)
    {
        m_fX /= l;
        m_fY /= l;
    }
    
    return *this;
}

float Vector2::angle()
{
    float radians = atan2f(m_fY, m_fX);
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
    
    float newX = m_fX * cos - m_fY * sin;
    float newY = m_fX * sin + m_fY * cos;
    
    m_fX = newX;
    m_fY = newY;
    
    return *this;
}

float Vector2::dot(const Vector2 &other)
{
    return (m_fX * other.m_fX + m_fY * other.m_fY);
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
    float distX = m_fX - x;
    float distY = m_fY - y;
    
    return distX * distX + distY * distY;
}

bool Vector2::isEqualTo(const Vector2& other)
{
    return (m_fX == other.m_fX && m_fY == other.m_fY);
}

float Vector2::getX() const
{
    return m_fX;
}

float Vector2::getY() const
{
    return m_fY;
}

float& Vector2::getXRef()
{
    return m_fX;
}

float& Vector2::getYRef()
{
    return m_fY;
}
