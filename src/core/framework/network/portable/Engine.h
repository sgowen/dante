//
//  Engine.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Engine__
#define __noctisgames__Engine__

#include <memory>

class Engine
{
public:
	virtual ~Engine();
	static std::unique_ptr<Engine> sInstance;
    
    virtual int Run();

	void SetShouldKeepRunning(bool inShouldKeepRunning) { mShouldKeepRunning = inShouldKeepRunning; }

    virtual void DoFrame();
    
protected:
    bool mShouldKeepRunning;
    
	Engine();
};

#endif /* defined(__noctisgames__Engine__) */
