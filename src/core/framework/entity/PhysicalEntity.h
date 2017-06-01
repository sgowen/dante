//
//  PhysicalEntity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PhysicalEntity__
#define __noctisgames__PhysicalEntity__

#include "Entity.h"

#include "Vector2.h"
#include "Color.h"

#include "RTTI.h"

#include <vector>

class NGRect;

class PhysicalEntity : public Entity
{
    RTTI_DECL;
    
public:
    PhysicalEntity(float x, float y, float width, float height);
    
    virtual ~PhysicalEntity();
    
    virtual void update();

    virtual void resetBounds(float width, float height);

    virtual void updateBounds();

    void setPosition(Vector2 position);
    
    Vector2& getPosition();
    
    Vector2& getVelocity();
    
    Vector2& getAcceleration();

    std::vector<NGRect *>& getBounds();
    
    NGRect& getMainBounds();
    
    void setColor(Color color);
    
    Color& getColor();
    
    const float& getWidth();
    
    void setWidth(float width);
    
    const float& getHeight();
    
    void setHeight(float height);
    
    void setAngle(float angle);
    
    float getAngle();
    
protected:
    Vector2 m_position;
    Vector2 m_velocity;
    Vector2 m_acceleration;
	std::vector<NGRect *> m_bounds;
    Color m_color;
    float m_fWidth;
    float m_fHeight;
    float m_fAngle;
};

#endif /* defined(__noctisgames__PhysicalEntity__) */
