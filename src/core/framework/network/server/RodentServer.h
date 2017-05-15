class RodentServer : public Rodent
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new RodentServer()); }
	void HandleDying() override;
	virtual bool		HandleCollisionWithCat(RoboCat* inCat) override;

protected:
	RodentServer();

};
