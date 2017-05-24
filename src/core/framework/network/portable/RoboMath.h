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
    static const Vector3 UnitX;
    static const Vector3 UnitY;
    static const Vector3 UnitZ;
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
    const float PI = 3.1415926535f;
    float GetRandomFloat();
    
    Vector3 GetRandomVector(const Vector3& inMin, const Vector3& inMax);
    
    inline bool	Is2DVectorEqual(const Vector3& inA, const Vector3& inB)
    {
        return (inA.m_fX == inB.m_fX && inA.m_fY == inB.m_fY);
    }
    
    inline float ToDegrees(float inRadians)
    {
        return inRadians * 180.0f / PI;
    }
}

namespace Colors
{
    static const Vector3 Black(0.0f, 0.0f, 0.0f);
    static const Vector3 White(1.0f, 1.0f, 1.0f);
    static const Vector3 Red(1.0f, 0.0f, 0.0f);
    static const Vector3 Green(0.0f, 1.0f, 0.0f);
    static const Vector3 Blue(0.0f, 0.0f, 1.0f);
    static const Vector3 LightYellow(1.0f, 1.0f, 0.88f);
    static const Vector3 LightBlue(0.68f, 0.85f, 0.9f);
    static const Vector3 LightPink(1.0f, 0.71f, 0.76f);
    static const Vector3 LightGreen(0.56f, 0.93f, 0.56f);
}

#endif /* defined(__noctisgames__RoboMath__) */
