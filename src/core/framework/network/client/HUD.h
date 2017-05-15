#include "RoboCatClientPCH.h"

class HUD
{
public:
	static void StaticInit();
	static std::unique_ptr< HUD > sInstance;

	void SetPlayerHealth(int inHealth) { mHealth = inHealth; }

    Vector3 mBandwidthOrigin;
    Vector3 mRoundTripTimeOrigin;
    Vector3 mScoreBoardOrigin;
    Vector3 mScoreOffset;
    Vector3 mHealthOffset;
    int mHealth;
    
private:
	HUD();
};
