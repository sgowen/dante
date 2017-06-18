//
//  KeyboardLookup.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__KeyboardLookup__
#define __noctisgames__KeyboardLookup__

#define NG_KEY_BACK_SPACE 0x0008

#define NG_KEY_CARRIAGE_RETURN 0x000D

#define NG_KEY_ESCAPE 0x001B

#define NG_KEY_SPACE_BAR 0x0020

#define NG_KEY_PERIOD 0x002E

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

#define NG_KEY_DELETE 0x007F

#define NG_KEY_ARROW_UP 0xF700
#define NG_KEY_ARROW_DOWN 0xF701
#define NG_KEY_ARROW_LEFT 0xF702
#define NG_KEY_ARROW_RIGHT 0xF703

inline bool isKeySupported(unsigned short key)
{
    switch (key)
    {
        case NG_KEY_BACK_SPACE:
        case NG_KEY_CARRIAGE_RETURN:
        case NG_KEY_ESCAPE:
        case NG_KEY_SPACE_BAR:
        case NG_KEY_PERIOD:
        case NG_KEY_ZERO:
        case NG_KEY_ONE:
        case NG_KEY_TWO:
        case NG_KEY_THREE:
        case NG_KEY_FOUR:
        case NG_KEY_FIVE:
        case NG_KEY_SIX:
        case NG_KEY_SEVEN:
        case NG_KEY_EIGHT:
        case NG_KEY_NINE:
        case NG_KEY_COLON:
        case NG_KEY_A:
        case NG_KEY_B:
        case NG_KEY_C:
        case NG_KEY_D:
        case NG_KEY_E:
        case NG_KEY_F:
        case NG_KEY_G:
        case NG_KEY_H:
        case NG_KEY_I:
        case NG_KEY_J:
        case NG_KEY_K:
        case NG_KEY_L:
        case NG_KEY_M:
        case NG_KEY_N:
        case NG_KEY_O:
        case NG_KEY_P:
        case NG_KEY_Q:
        case NG_KEY_R:
        case NG_KEY_S:
        case NG_KEY_T:
        case NG_KEY_U:
        case NG_KEY_V:
        case NG_KEY_W:
        case NG_KEY_X:
        case NG_KEY_Y:
        case NG_KEY_Z:
        case NG_KEY_DELETE:
        case NG_KEY_ARROW_UP:
        case NG_KEY_ARROW_DOWN:
        case NG_KEY_ARROW_LEFT:
        case NG_KEY_ARROW_RIGHT:
            return true;
        default:
            return false;
    }
}

#endif /* defined(__noctisgames__KeyboardLookup__) */
