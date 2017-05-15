#include "RoboCatClientPCH.h"

class RodentClient : public Rodent
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr(new RodentClient()); }

protected:
	RodentClient();

private:

//	SpriteComponentPtr	mSpriteComponent;
};
