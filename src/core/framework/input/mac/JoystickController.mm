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
#include "FrameworkConstants.h"

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

@implementation JoystickController

- (instancetype)init
{
    self = [super init];
    
    if (self)
    {
        mJoysticks = [[NSMutableArray alloc] init];
        mJoystickButtons = [[NSMutableArray alloc] init];
        mLastDPadEvents = [[NSMutableArray alloc] init];
        
        for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            DDHidJoystick* placeHolder = [[DDHidJoystick alloc] init];
            [placeHolder setTag:INPUT_UNASSIGNED];
            [mJoysticks addObject:placeHolder];
            
            [mJoystickButtons addObject:[[NSMutableArray alloc] init]];
            
            [mLastDPadEvents addObject:[NSNumber numberWithInt:GamePadEventType_NONE]];
        }
    }
    
    return self;
}

- (bool)isNewJoystick:(DDHidJoystick*)newJs
{
    for (DDHidJoystick* cjs in mJoysticks)
    {
        if ([cjs tag] != INPUT_UNASSIGNED && [cjs locationId] == [newJs locationId])
        {
            return false;
        }
    }
    
    return true;
}

- (bool)hasJoystickBeenRemoved:(NSArray*)master joystick:(DDHidJoystick*)js
{
    for (DDHidJoystick* mjs in master)
    {
        if ([mjs locationId] == [js locationId])
        {
            return false;
        }
    }
    
    return true;
}

- (void)scan
{
    [self performSelector:@selector(scan) withObject:nil afterDelay:5];
    
    NSArray* joysticks = [DDHidJoystick allJoysticks];
    
    for (DDHidJoystick* js in mJoysticks)
    {
        int index = [js tag];
        if (index == INPUT_UNASSIGNED)
        {
            continue;
        }
        
        if ([self hasJoystickBeenRemoved:joysticks joystick:js])
        {
            if (index != INPUT_UNASSIGNED)
            {
                [mJoystickButtons[index] removeAllObjects];
            }
            
            [js setTag:INPUT_UNASSIGNED];
            
            if ([js isListening])
            {
                [js stopListening];
            }
        }
    }
    
    for (DDHidJoystick* js in joysticks)
    {
        if ([self isNewJoystick:js])
        {
            [self activate:js];
        }
    }
}

- (void)activate:(DDHidJoystick*)joystick
{
    for (int i = 0; i < [mJoysticks count]; ++i)
    {
        DDHidJoystick *currentJoystick = [mJoysticks objectAtIndex:i];
        if ([currentJoystick tag] == INPUT_UNASSIGNED)
        {
            [joystick setDelegate:self];
            [mJoysticks replaceObjectAtIndex:i withObject:joystick];
            [self listenOnJoystick:i];
            return;
        }
    }
}

- (void)listenOnJoystick:(NSUInteger)index
{
    if (index != NSNotFound)
    {
        DDHidJoystick *currentJoystick = [mJoysticks objectAtIndex:index];
        [currentJoystick setTag:(int)index];
        [currentJoystick startListening];
        
        [mJoystickButtons[index] removeAllObjects];
        
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
    int index = [joystick tag];
    
    if (value == -1)
    {
        if ([mLastDPadEvents[index] intValue] != GamePadEventType_NONE)
        {
            GAME_PAD_INPUT_MANAGER->onInput((GamePadEventType)[mLastDPadEvents[index] intValue], [joystick tag], 0, 0);
        }
        
        mLastDPadEvents[index] = [NSNumber numberWithInt:GamePadEventType_NONE];
    }
    else
    {
        if (value == 0)
        {
            mLastDPadEvents[index] = [NSNumber numberWithInt:GamePadEventType_D_PAD_UP];
        }
        else if (value == 9000)
        {
            mLastDPadEvents[index] = [NSNumber numberWithInt:GamePadEventType_D_PAD_RIGHT];
        }
        else if (value == 18000)
        {
            mLastDPadEvents[index] = [NSNumber numberWithInt:GamePadEventType_D_PAD_DOWN];
        }
        else if (value == 27000)
        {
            mLastDPadEvents[index] = [NSNumber numberWithInt:GamePadEventType_D_PAD_LEFT];
        }
        
        GAME_PAD_INPUT_MANAGER->onInput((GamePadEventType)[mLastDPadEvents[index] intValue], [joystick tag], 1, 0);
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
    else if (buttonNumber == 4)
    {
        // Left Bumper on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, index, 1, 0);
    }
    else if (buttonNumber == 5)
    {
        // Right Bumper on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, index, 1, 0);
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
    else if (buttonNumber == 4)
    {
        // Left Bumper on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_LEFT, index, 0, 0);
    }
    else if (buttonNumber == 5)
    {
        // Right Bumper on PS4 controller
        GAME_PAD_INPUT_MANAGER->onInput(GamePadEventType_BUMPER_RIGHT, index, 0, 0);
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
