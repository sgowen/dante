//
//  MathUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/28/16.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MathUtil__
#define __noctisgames__MathUtil__

#include <math.h>
#include <float.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

inline float clamp(float x, float upper, float lower)
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

#endif /* defined(__noctisgames__MathUtil__) */
