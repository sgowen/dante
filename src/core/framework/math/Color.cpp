//
//  Color.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/18/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/math/Color.h"

Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::DARK_RED = Color(0.6f, 0.1f, 0.1f, 1.0f);

Color::Color(float redIn, float greenIn, float blueIn, float alphaIn) : red(redIn), green(greenIn), blue(blueIn), alpha(alphaIn)
{
    // Empty
}

Color::Color() : red(0), green(0), blue(0), alpha(0)
{
    // Empty
}
