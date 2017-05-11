#ifndef __noctisgames__Client__
#define __noctisgames__Client__

#include "RoboCatClientPCH.h"

class Client : public Engine
{
public:
	static bool StaticInit();

    virtual void DoFrame() override;
    
protected:
	Client();
};

#endif /* defined(__noctisgames__Client__) */
