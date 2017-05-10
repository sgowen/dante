class Server : public Engine
{
public:

	static bool StaticInit();

	virtual int Run();

	void HandleNewClient( ClientProxyPtr inClientProxy );
	void HandleLostClient( ClientProxyPtr inClientProxy );

	RoboCatPtr	GetCatForPlayer( int inPlayerId );
	void	SpawnCatForPlayer( int inPlayerId );


private:
    float m_fFrameStateTime;
    
	Server();

	bool	InitNetworkManager();
	void	SetupWorld();

};
