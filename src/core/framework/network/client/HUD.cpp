#include "RoboCatClientPCH.h"

std::unique_ptr< HUD >	HUD::sInstance;

HUD::HUD() :
mScoreBoardOrigin( 1.f, 7.f, 0.0f ),
mBandwidthOrigin( 4.f, 6.6f, 0.0f ),
mRoundTripTimeOrigin( 5.f, 7.f, 0.0f ),
mScoreOffset( 0.f, -0.4f, 0.0f ),
mHealthOffset( 9.f, 7.f, 0.0f ),
mHealth( 0 )
{
    // Empty
}

void HUD::StaticInit()
{
	sInstance.reset( new HUD() );
}
