//
//  ViewController.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import "ViewController.h"

#include "game/logic/MainEngine.h"

@implementation ViewController

- (void)viewDidLoad
{
    _engine = new MainEngine();
    
    [super viewDidLoad];
}

@end
