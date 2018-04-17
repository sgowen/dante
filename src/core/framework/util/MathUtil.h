//
//  MathUtil.h
//  noctisgames
//
//  Created by Stephen Gowen on 7/28/16.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MathUtil__
#define __noctisgames__MathUtil__

#include <Box2D/Common/b2Math.h>
#include <framework/graphics/portable/VertexProgramInput.h>

#include <math.h>
#include <float.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define NG_M_PI 3.1415926535f

#define DEGREES_TO_RADIANS(angle) ( (angle) / 180.0f * NG_M_PI )
#define RADIANS_TO_DEGREES(angle) ( (angle) / NG_M_PI * 180.0f )

inline float clamp(float x, float lower, float upper)
{
    return fminf(upper, fmaxf(x, lower));
}

inline float sanitizeCloseToZeroValue(float x)
{
    if (x < 0.1f && x > -0.1f)
    {
        return 0.0f;
    }
    
    return x;
}

inline bool areFloatsPracticallyEqual(float A, float B, float maxDiff = 0.000001f, float maxRelDiff = FLT_EPSILON)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float diff = fabs(A - B);
    if (diff <= maxDiff)
    {
        return true;
    }
    
    A = fabs(A);
    B = fabs(B);
    
    float largest = (B > A) ? B : A;
    
    if (diff <= largest * maxRelDiff)
    {
        return true;
    }
    
    return false;
}

inline bool isCloseEnough(const b2Vec2& inA, const b2Vec2& inB)
{
    return areFloatsPracticallyEqual(inA.x, inB.x) && areFloatsPracticallyEqual(inA.y, inB.y);
}

inline bool isCloseEnough(float A, float B)
{
    return areFloatsPracticallyEqual(A, B);
}

inline b2Vec2 lerpBox2DVector(b2Vec2& inA, const b2Vec2& inB, float t)
{
    return b2Vec2(inA + t * (inB - inA));
}

inline void mat4x4_identity(mat4x4 M)
{
    int i, j;
    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            M[i][j] = i==j ? 1.f : 0.f;
        }
    }
}

inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
{
    M[0][0] = 2.f/(r-l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;
    
    M[1][1] = 2.f/(t-b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;
    
    M[2][2] = -2.f/(f-n);
    M[2][0] = M[2][1] = M[2][3] = 0.f;
    
    M[3][0] = -(r+l)/(r-l);
    M[3][1] = -(t+b)/(t-b);
    M[3][2] = -(f+n)/(f-n);
    M[3][3] = 1.f;
}

#endif /* defined(__noctisgames__MathUtil__) */
