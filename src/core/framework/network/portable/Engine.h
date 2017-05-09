#include <RoboCatShared.h>

class Engine
{
public:

	virtual ~Engine();
	static std::unique_ptr< Engine >	sInstance;

	virtual int		Run();
	void			SetShouldKeepRunning( bool inShouldKeepRunning ) { mShouldKeepRunning = inShouldKeepRunning; }
	virtual void	HandleEvent( int inEvent );

protected:

	Engine();

	virtual void	DoFrame();

private:


			
			int		DoRunLoop();

			bool	mShouldKeepRunning;



};
