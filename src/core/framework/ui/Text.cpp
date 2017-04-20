//
//  Text.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 10/6/13.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Text.h"

#include "MathUtil.h"

Text::Text(std::string text, float x, float y, float width, float height, float red, float green, float blue, float alpha) : PhysicalEntity(x, y, width, height),
m_strText(text),
m_color(red, green, blue, alpha)
{
    updateCharColors();
}

std::string Text::getText()
{
	return m_strText;
}

void Text::setText(std::string text)
{
	m_strText = text;
    
    updateCharColors();
}

Color& Text::getColor()
{
    return m_color;
}

void Text::setColor(float red, float green, float blue, float alpha)
{
	m_color.red = clamp(red, 1, 0);
	m_color.green = clamp(green, 1, 0);
	m_color.blue = clamp(blue, 1, 0);
	m_color.alpha = clamp(alpha, 1, 0);
    
    updateCharColors();
}

std::vector<Color>& Text::getCharColors()
{
    return m_charColors;
}

void Text::setCharColor(int charIndex, float red, float green, float blue, float alpha)
{
    m_charColors.at(charIndex).red = clamp(red, 1, 0);
    m_charColors.at(charIndex).green = clamp(green, 1, 0);
    m_charColors.at(charIndex).blue = clamp(blue, 1, 0);
    m_charColors.at(charIndex).alpha = clamp(alpha, 1, 0);
}

void Text::updateCharColors()
{
    m_charColors.clear();
    
    unsigned long len = m_strText.length();
    
    for (unsigned int i = 0; i < len; ++i)
    {
        m_charColors.push_back(Color(m_color.red, m_color.green, m_color.blue, m_color.alpha));
    }
}
