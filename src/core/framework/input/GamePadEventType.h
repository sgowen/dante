//
//  GamePadEventType.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GamePadEventType__
#define __noctisgames__GamePadEventType__

typedef enum
{
    GamePadEventType_NONE,
    GamePadEventType_D_PAD_RIGHT,
    GamePadEventType_D_PAD_UP,
    GamePadEventType_D_PAD_LEFT,
    GamePadEventType_D_PAD_DOWN,
    GamePadEventType_A_BUTTON,
    GamePadEventType_B_BUTTON,
    GamePadEventType_X_BUTTON,
    GamePadEventType_Y_BUTTON,
    GamePadEventType_BUMPER_LEFT,
    GamePadEventType_BUMPER_RIGHT,
	GamePadEventType_START_BUTTON,
    GamePadEventType_BACK_BUTTON,
    GamePadEventType_STICK_LEFT,
    GamePadEventType_STICK_RIGHT,
    GamePadEventType_TRIGGER,
} GamePadEventType;

#endif /* defined(__noctisgames__GamePadEventType__) */
