//
//  Color.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/18/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/Color.h>

Color Color::WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::GREEN = Color(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::HALF = Color(0.5f, 0.5f, 0.5f, 0.5f);
Color Color::DOUBLE = Color(2.0f, 2.0f, 2.0f, 1.0f);

Color::Color(float redIn, float greenIn, float blueIn, float alphaIn) : red(redIn), green(greenIn), blue(blueIn), alpha(alphaIn)
{
    // Empty
}

Color::Color() : red(0), green(0), blue(0), alpha(0)
{
    // Empty
}
