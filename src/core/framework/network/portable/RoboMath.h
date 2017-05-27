//
//  RoboMath.h
//  noctisgames-framework
//
//  Created by Joshua Glazer on 6/13/15.
//  Copyright (c) 2015 com.JoshuaGlazer.Book. All rights reserved.
//

#ifndef __noctisgames__RoboMath__
#define __noctisgames__RoboMath__

#include <math.h>

class Vector3
{
public:
    float m_fX, m_fY, m_fZ;
    
    Vector3(float x, float y, float z) :
    m_fX(x),
    m_fY(y),
    m_fZ(z)
    {}
    
    Vector3() :
    m_fX(0.0f),
    m_fY(0.0f),
    m_fZ(0.0f)
    {}
    
    void Set(float x, float y, float z)
    {
        m_fX = x;
        m_fY = y;
        m_fZ = z;
    }
    
    friend Vector3 operator+(const Vector3& inLeft, const Vector3& inRight)
    {
        return Vector3(inLeft.m_fX + inRight.m_fX, inLeft.m_fY + inRight.m_fY, inLeft.m_fZ + inRight.m_fZ);
    }
    
    friend Vector3 operator-(const Vector3& inLeft, const Vector3& inRight)
    {
        return Vector3(inLeft.m_fX - inRight.m_fX, inLeft.m_fY - inRight.m_fY, inLeft.m_fZ - inRight.m_fZ);
    }
    
    // Component-wise multiplication
    friend Vector3 operator*(const Vector3& inLeft, const Vector3& inRight)
    {
        return Vector3(inLeft.m_fX * inRight.m_fX, inLeft.m_fY * inRight.m_fY, inLeft.m_fZ * inRight.m_fZ);
    }
    
    // Scalar multiply
    friend Vector3 operator*(float inScalar, const Vector3& inVec)
    {
        return Vector3(inVec.m_fX * inScalar, inVec.m_fY * inScalar, inVec.m_fZ * inScalar);
    }
    
    friend Vector3 operator*(const Vector3& inVec, float inScalar)
    {
        return Vector3(inVec.m_fX * inScalar, inVec.m_fY * inScalar, inVec.m_fZ * inScalar);
    }
    
    Vector3& operator*=(float inScalar)
    {
        m_fX *= inScalar;
        m_fY *= inScalar;
        m_fZ *= inScalar;
        return *this;
    }
    
    Vector3& operator+=(const Vector3& inRight)
    {
        m_fX += inRight.m_fX;
        m_fY += inRight.m_fY;
        m_fZ += inRight.m_fZ;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& inRight)
    {
        m_fX -= inRight.m_fX;
        m_fY -= inRight.m_fY;
        m_fZ -= inRight.m_fZ;
        return *this;
    }
    
    float Length()
    {
        return sqrtf(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
    }
    
    float LengthSq()
    {
        return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
    }
    
    float Length2D()
    {
        return sqrtf(m_fX * m_fX + m_fY * m_fY);
    }
    
    float LengthSq2D()
    {
        return m_fX * m_fX + m_fY * m_fY;
    }
    
    void Normalize()
    {
        float length = Length();
        m_fX /= length;
        m_fY /= length;
        m_fZ /= length;
    }
    
    void Normalize2D()
    {
        float length = Length2D();
        m_fX /= length;
        m_fY /= length;
    }
    
    friend float Dot(const Vector3& inLeft, const Vector3& inRight)
    {
        return (inLeft.m_fX * inRight.m_fX + inLeft.m_fY * inRight.m_fY + inLeft.m_fZ * inRight.m_fZ);
    }
    
    friend float Dot2D(const Vector3& inLeft, const Vector3& inRight)
    {
        return (inLeft.m_fX * inRight.m_fX + inLeft.m_fY * inRight.m_fY);
    }
    
    friend Vector3 Cross(const Vector3& inLeft, const Vector3& inRight)
    {
        Vector3 temp;
        temp.m_fX = inLeft.m_fY * inRight.m_fZ - inLeft.m_fZ * inRight.m_fY;
        temp.m_fY = inLeft.m_fZ * inRight.m_fX - inLeft.m_fX * inRight.m_fZ;
        temp.m_fZ = inLeft.m_fX * inRight.m_fY - inLeft.m_fY * inRight.m_fX;
        return temp;
    }
    
    friend Vector3 Lerp(const Vector3& inA, const Vector3& inB, float t)
    {
        return Vector3(inA + t * (inB - inA));
    }
    
    static const Vector3 Zero;
};

template< int tValue, int tBits >
struct GetRequiredBitsHelper
{
    enum { Value = GetRequiredBitsHelper<(tValue >> 1), tBits + 1 >::Value };
};

template< int tBits >
struct GetRequiredBitsHelper<0, tBits >
{
    enum { Value = tBits };
};

template< int tValue >
struct GetRequiredBits
{
    enum { Value = GetRequiredBitsHelper<tValue, 0 >::Value };
};

namespace RoboMath
{
    inline bool	Is2DVectorEqual(const Vector3& inA, const Vector3& inB)
    {
        return (inA.m_fX == inB.m_fX && inA.m_fY == inB.m_fY);
    }
}

#include "Color.h"

namespace Colors
{
    static Color Black(0.0f, 0.0f, 0.0f, 1);
    static Color White(1.0f, 1.0f, 1.0f, 1);
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color RedHalfVisible(1.0f, 0.0f, 0.0f, 0.3f);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color LightYellow(1.0f, 1.0f, 0.88f, 1);
    static Color LightBlue(0.68f, 0.85f, 0.9f, 1);
    static Color LightPink(1.0f, 0.71f, 0.76f, 1);
    static Color LightGreen(0.56f, 0.93f, 0.56f, 1);
}

#endif /* defined(__noctisgames__RoboMath__) */
