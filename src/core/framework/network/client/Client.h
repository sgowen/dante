#ifndef __noctisgames__Client__
#define __noctisgames__Client__

#include <RoboCatClientPCH.h>

class Client : public Engine
{
public:

	static bool StaticInit( );

protected:

	Client();

	virtual void	DoFrame() override;
	virtual void	HandleEvent( int inEvent ) override;

private:



};

#endif /* defined(__noctisgames__Client__) */
