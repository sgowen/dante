//
//  StringUtils.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StringUtils__
#define __noctisgames__StringUtils__

#include "RoboCatShared.h"

namespace StringUtils
{
	string GetCommandLineArg(int inIndex);

	string Sprintf(const char* inFormat, ...);

	void	Log(const char* inFormat);
	void	Log(const char* inFormat, ...);
}

#define LOG(...) StringUtils::Log(__VA_ARGS__);

#endif /* defined(__noctisgames__StringUtils__) */
