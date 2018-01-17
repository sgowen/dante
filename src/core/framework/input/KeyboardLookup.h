//
//  KeyboardLookup.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__KeyboardLookup__
#define __noctisgames__KeyboardLookup__

#include <algorithm>

#define NG_KEY_SPACE_BAR 0x0020

#define NG_KEY_ASCII_COMMA 0x002C

#define NG_KEY_ASCII_PERIOD 0x002E

#define NG_KEY_ZERO 0x0030
#define NG_KEY_ONE 0x0031
#define NG_KEY_TWO 0x0032
#define NG_KEY_THREE 0x0033
#define NG_KEY_FOUR 0x0034
#define NG_KEY_FIVE 0x0035
#define NG_KEY_SIX 0x0036
#define NG_KEY_SEVEN 0x0037
#define NG_KEY_EIGHT 0x0038
#define NG_KEY_NINE 0x0039

#define NG_KEY_COLON 0x003A

#define NG_KEY_A 0x0041
#define NG_KEY_B 0x0042
#define NG_KEY_C 0x0043
#define NG_KEY_D 0x0044
#define NG_KEY_E 0x0045
#define NG_KEY_F 0x0046
#define NG_KEY_G 0x0047
#define NG_KEY_H 0x0048
#define NG_KEY_I 0x0049
#define NG_KEY_J 0x004A
#define NG_KEY_K 0x004B
#define NG_KEY_L 0x004C
#define NG_KEY_M 0x004D
#define NG_KEY_N 0x004E
#define NG_KEY_O 0x004F
#define NG_KEY_P 0x0050
#define NG_KEY_Q 0x0051
#define NG_KEY_R 0x0052
#define NG_KEY_S 0x0053
#define NG_KEY_T 0x0054
#define NG_KEY_U 0x0055
#define NG_KEY_V 0x0056
#define NG_KEY_W 0x0057
#define NG_KEY_X 0x0058
#define NG_KEY_Y 0x0059
#define NG_KEY_Z 0x005A

#define NG_KEY_CTRL 341
#define NG_KEY_CMD 343

#define NG_KEY_DELETE 0x007F

#ifdef _WIN32
    #define NG_KEY_BACK_SPACE 0x0008
    #define NG_KEY_ESCAPE 0x001B
    #define NG_KEY_PERIOD 0xbe
    #define NG_KEY_COMMA 0xbc
    #define NG_KEY_ARROW_LEFT 0x25
    #define NG_KEY_ARROW_UP 0x26
    #define NG_KEY_ARROW_RIGHT 0x27
    #define NG_KEY_ARROW_DOWN 0x28
    #define NG_KEY_CARRIAGE_RETURN 0x000D
#else
    #define NG_KEY_BACK_SPACE 0x0103
    #define NG_KEY_ESCAPE 0x0100
    #define NG_KEY_PERIOD NG_KEY_ASCII_PERIOD
    #define NG_KEY_COMMA NG_KEY_ASCII_COMMA
    #define NG_KEY_ARROW_UP 0x0109
    #define NG_KEY_ARROW_DOWN 0x0108
    #define NG_KEY_ARROW_LEFT 0x0107
    #define NG_KEY_ARROW_RIGHT 0x0106
    #define NG_KEY_CARRIAGE_RETURN 0x0101
#endif

inline std::vector<unsigned short>& getAllSupportedKeys()
{
	static std::vector<unsigned short> keys;
	if (keys.size() == 0)
	{
		keys.push_back(NG_KEY_BACK_SPACE);
		keys.push_back(NG_KEY_CARRIAGE_RETURN);
		keys.push_back(NG_KEY_ESCAPE);
		keys.push_back(NG_KEY_SPACE_BAR);
		keys.push_back(NG_KEY_ASCII_PERIOD);
		keys.push_back(NG_KEY_ASCII_COMMA);
        keys.push_back(NG_KEY_PERIOD);
		keys.push_back(NG_KEY_COMMA);
		keys.push_back(NG_KEY_ZERO);
		keys.push_back(NG_KEY_ONE);
		keys.push_back(NG_KEY_TWO);
		keys.push_back(NG_KEY_THREE);
		keys.push_back(NG_KEY_FOUR);
		keys.push_back(NG_KEY_FIVE);
		keys.push_back(NG_KEY_SIX);
		keys.push_back(NG_KEY_SEVEN);
		keys.push_back(NG_KEY_EIGHT);
		keys.push_back(NG_KEY_NINE);
		keys.push_back(NG_KEY_COLON);
		keys.push_back(NG_KEY_A);
		keys.push_back(NG_KEY_B);
		keys.push_back(NG_KEY_C);
		keys.push_back(NG_KEY_D);
		keys.push_back(NG_KEY_E);
		keys.push_back(NG_KEY_F);
		keys.push_back(NG_KEY_G);
		keys.push_back(NG_KEY_H);
		keys.push_back(NG_KEY_I);
		keys.push_back(NG_KEY_J);
		keys.push_back(NG_KEY_K);
		keys.push_back(NG_KEY_L);
		keys.push_back(NG_KEY_M);
		keys.push_back(NG_KEY_N);
		keys.push_back(NG_KEY_O);
		keys.push_back(NG_KEY_P);
		keys.push_back(NG_KEY_Q);
		keys.push_back(NG_KEY_R);
		keys.push_back(NG_KEY_S);
		keys.push_back(NG_KEY_T);
		keys.push_back(NG_KEY_U);
		keys.push_back(NG_KEY_V);
		keys.push_back(NG_KEY_W);
		keys.push_back(NG_KEY_X);
		keys.push_back(NG_KEY_Y);
		keys.push_back(NG_KEY_Z);
        keys.push_back(NG_KEY_CTRL);
        keys.push_back(NG_KEY_CMD);
		keys.push_back(NG_KEY_DELETE);
		keys.push_back(NG_KEY_ARROW_UP);
		keys.push_back(NG_KEY_ARROW_DOWN);
		keys.push_back(NG_KEY_ARROW_LEFT);
		keys.push_back(NG_KEY_ARROW_RIGHT);
	}

	return keys;
}

inline bool isKeySupported(unsigned short key)
{
	std::vector<unsigned short>& keys = getAllSupportedKeys();

	return std::find(keys.begin(), keys.end(), key) != keys.end();
}

#endif /* defined(__noctisgames__KeyboardLookup__) */
