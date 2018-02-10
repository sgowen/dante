//
//  Font.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/Font.h>

#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/graphics/portable/Color.h>

Font::Font(int offsetX, int offsetY, int glyphsPerRow, int glyphWidth, int glyphHeight, int textureWidth, int textureHeight)
{
	int x = offsetX;
	int y = offsetY;

	for (int i = 0; i < 176; ++i)
	{
		_glyphs.push_back(TextureRegion("font", x, y, glyphWidth, glyphHeight, textureWidth, textureHeight));

		x += glyphWidth;

		if (x == offsetX + glyphsPerRow * glyphWidth)
		{
			x = offsetX;
			y += glyphHeight;
		}
	}
}

Font::~Font()
{
    _glyphs.clear();
}

void Font::renderText(SpriteBatcher &spriteBatcher, std::string &text, float x, float y, float width, float height, int justification)
{
    unsigned long len = text.length();
    
	if (justification == FONT_ALIGN_CENTER)
	{
        float result = width / 2;
		x -= len * result;
		x += width / 2;
	}
	else if (justification == FONT_ALIGN_RIGHT)
	{
		x -= (text.length() - 1) * width;
	}

	for (unsigned int i = 0; i < len; ++i)
	{
		int c = ((int)text.at(i));

        renderAsciiChar(spriteBatcher, c, x, y, width, height);

		x += width;
	}
}

void Font::renderAsciiChar(SpriteBatcher &spriteBatcher, int asciiChar, float x, float y, float width, float height)
{
    if (asciiChar < 0 || asciiChar > 175)
    {
        return;
    }
    
    spriteBatcher.renderSprite(x, y, width, height, 0, _glyphs[asciiChar]);
}
