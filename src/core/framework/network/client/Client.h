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
