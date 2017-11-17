//
//  Vector2.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Vector2__
#define __noctisgames__Vector2__

class Vector2
{
public:
    static const Vector2 Zero;
    
    friend Vector2 operator+(const Vector2& inA, const Vector2& inRight);
    friend Vector2 operator-(const Vector2& inA, const Vector2& inRight);
    friend Vector2 operator*(float inScalar, const Vector2& other);
    friend Vector2 operator*(const Vector2& other, float inScalar);
    friend Vector2 operator*(const Vector2& inA, const Vector2& inRight);
    
    friend Vector2 lerp(Vector2& inA, Vector2& inB, float t);
    
    Vector2(float x = 0, float y = 0);
    
    Vector2(const Vector2 &cSource);
    
    Vector2 set(const Vector2 &other);
    Vector2 set(float x, float y);
	Vector2 setX(float x);
	Vector2 setY(float y);

	Vector2 cpy();
    
    void operator+=(const Vector2 &other);
    Vector2 add(float x, float y);
    
    void operator-=(const Vector2 &other);
    Vector2 sub(float x, float y);
    
    Vector2 operator*=(float scalar);
    Vector2 mul(float scalar);
    
    float len();
    float lenSquared();
    
    Vector2 nor();
    
    float angle();
    
    Vector2 rotate(float angle);
    
    float dot(const Vector2 &other);
    
    float dist(const Vector2 &other) const;
    float dist(float x, float y) const;
    
    float distSquared(const Vector2 &other) const;
    float distSquared(float x, float y) const;
    
    bool isEqualTo(const Vector2& other);
    
    float getX() const;
    float getY() const;
    
    float& getXRef();
    float& getYRef();
    
private:
    float _x;
    float _y;
};

#endif /* defined(__noctisgames__Vector2__) */
