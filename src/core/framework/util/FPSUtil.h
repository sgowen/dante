//
//  FPSUtil.h
//  noctisgames-framework
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
    
    void update(float deltaTime);
    
    int getFPS() const;
    
private:
    float m_fStateTime;
    int m_iFPS;
    int m_iNumFrames;
    
    float getTime() const;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    FPSUtil();
    ~FPSUtil();
    FPSUtil(const FPSUtil&);
    FPSUtil& operator=(const FPSUtil&);
};

#endif /* defined(__noctisgames__FPSUtil__) */
