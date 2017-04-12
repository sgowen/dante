//
//  Text.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/6/13.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Text__
#define __noctisgames__Text__

#include "PhysicalEntity.h"

#include "Color.h"

#include <string>

class Text : public PhysicalEntity
{
public:
	Text(std::string text, float x, float y, float width, float height, float red, float green, float blue, float alpha);
    
	std::string getText();

	void setText(std::string text);

    Color& getColor();

	void setColor(float red, float green, float blue, float alpha);
    
    std::vector<Color>& getCharColors();
    
    void setCharColor(int charIndex, float red, float green, float blue, float alpha);

private:
	std::string m_strText;
    Color m_color;
    std::vector<Color> m_charColors;
	float m_fX;
	float m_fY;
	float m_fWidth;
	float m_fHeight;
    
    void updateCharColors();
};

#endif /* defined(__noctisgames__Text__) */
