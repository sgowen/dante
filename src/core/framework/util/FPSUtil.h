//
//  FPSUtil.h
//  noctisgames
//
//  Created by Stephen Gowen on 8/6/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FPSUtil__
#define __noctisgames__FPSUtil__

class FPSUtil
{
public:
    static FPSUtil* getInstance();
    
    void update(double deltaTime);
    
    int getFPS() const;
    
private:
    double _stateTime;
    int _fPS;
    int _numFrames;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    FPSUtil();
    ~FPSUtil();
    FPSUtil(const FPSUtil&);
    FPSUtil& operator=(const FPSUtil&);
};

#endif /* defined(__noctisgames__FPSUtil__) */
