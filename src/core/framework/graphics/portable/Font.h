//
//  Font.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Font__
#define __noctisgames__Font__

#include "framework/graphics/portable/FontAlign.h"

#include <string>
#include <vector>

class SpriteBatcher;
class TextureRegion;

class Font
{
public:
	Font(std::string textureName, int offsetX, int offsetY, int glyphsPerRow, int glyphWidth, int glyphHeight, int textureWidth, int textureHeight);
    ~Font();
    
	void renderText(SpriteBatcher &spriteBatcher, std::string &text, float x, float y, float width, float height, int justification = FONT_ALIGN_LEFT);
    void renderAsciiChar(SpriteBatcher &spriteBatcher, int asciiChar, float x, float y, float width, float height);

private:
    std::vector<TextureRegion> _glyphs;
};

#endif /* defined(__noctisgames__Font__) */
