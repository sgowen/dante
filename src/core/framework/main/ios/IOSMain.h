//
//  IOSMain.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "framework/main/portable/Engine.h"

@interface IOSMain : GLKViewController
{
@protected
    Engine *_engine;
}

@end
