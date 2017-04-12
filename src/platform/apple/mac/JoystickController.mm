/*
 * Copyright (c) 2007 Dave Dribin
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#import "JoystickController.h"

#import <DDHidLib/DDHidJoystick.h>
#import <DDHidLib/DDHidElement.h>

// C++
#include "GamePadInputManager.h"
#include "GamePadEventType.h"

@interface ButtonState : NSObject
{
    // Empty
}

@property (strong, nonatomic) NSString *name;
@property (assign, nonatomic) BOOL pressed;

- (id)initWithName:(NSString *)name;

@end

@implementation ButtonState

- (id)initWithName:(NSString *)name
{
    self = [super init];
    if (self)
    {
        self.name = name;
        self.pressed = NO;
    }
    
    return self;
}

@end

@interface JoystickController ()

@property (assign, nonatomic) GamePadEventType lastDPadEvent;

@end

@implementation JoystickController

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        self.lastDPadEvent = GamePadEventType_NONE;
        
        mJoystickButtons = [[NSMutableArray alloc] init];
        
        m_fStateTime = 0;
    }
    
    return self;
}

- (void)scan
{
    NSArray * joysticks = [DDHidJoystick allJoysticks];
    
    [mJoystickButtons removeAllObjects];
    
    [joysticks makeObjectsPerformSelector: @selector(setDelegate:)
                               withObject: self];
    
    mJoysticks = joysticks;
    
    unsigned long numPlayers = [mJoysticks count];
    for (int i = 0; i < numPlayers; ++i)
    {
        [self listenOnJoystick:i];
    }
}

//===========================================================
//  joysticks 
//=========================================================== 
- (NSArray *) joysticks
{
    return mJoysticks;
}

- (NSArray *) joystickButtons;
{
    return mJoystickButtons;
}

//=========================================================== 
//  joystickIndex 
//=========================================================== 
- (NSUInteger) joystickIndex
{
    return mJoystickIndex;
}

- (void)listenOnJoystick:(NSUInteger)index
{
    if (index != NSNotFound)
    {
        DDHidJoystick *currentJoystick = [mJoysticks objectAtIndex:index];
        [currentJoystick setTag:(int)index];
        [currentJoystick startListening];
        
        [mJoystickButtons addObject:[[NSMutableArray alloc] init]];
        
        NSArray * buttons = [currentJoystick buttonElements];
        NSEnumerator * e = [buttons objectEnumerator];
        DDHidElement * element;
        while (element = [e nextObject])
        {
            ButtonState * state = [[ButtonState alloc] initWithName: [[element usage] usageName]];
            [mJoystickButtons[index] addObject: state];
        }
    }
}

- (void)ddhidJoystick:(DDHidJoystick *)joystick
                 stick:(unsigned)stick
              xChanged:(int)value;
{
    float x = value;
    x /= 65536.0f;
    
    int index = [joystick tag] * 2;
    
    mJoystickAxis[index] = x;
    
    GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_LEFT, [joystick tag], mJoystickAxis[index], mJoystickAxis[index + 1]);
}

- (void)ddhidJoystick:(DDHidJoystick *)joystick
                 stick:(unsigned)stick
              yChanged:(int)value;
{
    float y = value;
    y /= 65536.0f;
    y *= -1;
    
    int index = [joystick tag] * 2;
    
    mJoystickAxis[index + 1] = y;
    
    GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_STICK_LEFT, [joystick tag], mJoystickAxis[index], mJoystickAxis[index + 1]);
}

- (void)ddhidJoystick:(DDHidJoystick *)joystick
                 stick:(unsigned)stick
             otherAxis:(unsigned)otherAxis
          valueChanged:(int)value;
{
    // Empty
}

- (void)ddhidJoystick:(DDHidJoystick *)joystick
                 stick:(unsigned)stick
             povNumber:(unsigned)povNumber
          valueChanged:(int)value;
{
    if (value == -1)
    {
        if (self.lastDPadEvent != GamePadEventType_NONE)
        {
            GAME_PAD_INPUT_MANAGER->onInput(self.lastDPadEvent, [joystick tag], 0, 0);
        }
        
        self.lastDPadEvent = GamePadEventType_NONE;
    }
    else
    {
        if (value == 0)
        {
            self.lastDPadEvent = GamePadEventType_D_PAD_UP;
        }
        else if (value > 0 && value < 18000)
        {
            self.lastDPadEvent = GamePadEventType_D_PAD_RIGHT;
        }
        else if (value > 9000 && value < 27000)
        {
            self.lastDPadEvent = GamePadEventType_D_PAD_DOWN;
        }
        else if (value > 18000)
        {
            self.lastDPadEvent = GamePadEventType_D_PAD_LEFT;
        }
        
        GAME_PAD_INPUT_MANAGER->onInput(self.lastDPadEvent, [joystick tag], 1, 0);
    }
}

- (void)ddhidJoystick:(DDHidJoystick *)joystick buttonDown:(unsigned)buttonNumber;
{
    int index = [joystick tag];
    
    NSMutableArray *buttonStates = [mJoystickButtons objectAtIndex:index];
    ButtonState * state = [buttonStates objectAtIndex: buttonNumber];
    [state setPressed: YES];
    
    if (buttonNumber == 0)
    {
        // Square on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, index, 1, 0);
    }
    else if (buttonNumber == 1)
    {
        // X on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, index, 1, 0);
    }
    else if (buttonNumber == 2)
    {
        // Circle on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, index, 1, 0);
    }
    else if (buttonNumber == 6)
    {
        // Left Trigger on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, index, 1, 0);
    }
    else if (buttonNumber == 7)
    {
        // Right Trigger on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, index, 0, 1);
    }
    else if (buttonNumber == 8)
    {
        // Select on PS4 cont
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, index, 1, 0);
    }
    else if (buttonNumber == 9)
    {
        // Start on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, index, 1, 0);
    }
}

- (void) ddhidJoystick:(DDHidJoystick *) joystick buttonUp:(unsigned)buttonNumber;
{
    int index = [joystick tag];
    NSMutableArray *buttonStates = [mJoystickButtons objectAtIndex:index];
    ButtonState * state = [buttonStates objectAtIndex: buttonNumber];
    [state setPressed: NO];
    
    if (buttonNumber == 0)
    {
        // Square on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_X_BUTTON, index, 0, 0);
    }
    else if (buttonNumber == 1)
    {
        // X on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_A_BUTTON, index, 0, 0);
    }
    else if (buttonNumber == 2)
    {
        // Circle on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_B_BUTTON, index, 0, 0);
    }
    else if (buttonNumber == 6)
    {
        // Left Trigger on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, index, 0, 0);
    }
    else if (buttonNumber == 7)
    {
        // Right Trigger on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_TRIGGER, index, 0, 0);
    }
    else if (buttonNumber == 8)
    {
        // Select on PS4 cont
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BACK_BUTTON, index, 0, 0);
    }
    else if (buttonNumber == 9)
    {
        // Start on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_START_BUTTON, index, 0, 0);
    }
}

@end
