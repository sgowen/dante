//
//  StringUtils.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "StringUtils.h"

#include <stdarg.h>

#if !_WIN32
void OutputDebugStringA(const char* inString)
{
	printf("%s", inString);
}
#endif

std::string StringUtils::Sprintf(const char* inFormat, ...)
{
	//not thread safe...
	static char temp[4096];
	
	va_list args;
	va_start (args, inFormat);
	
#if _WIN32
	_vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
	vsnprintf(temp, 4096, inFormat, args);
#endif
	return std::string(temp);
}

void StringUtils::Log(const char* inFormat, ...)
{
	//not thread safe...
	static char temp[4096];
	
	va_list args;
	va_start (args, inFormat);
	
#if _WIN32
	_vsnprintf_s(temp, 4096, 4096, inFormat, args);
#else
	vsnprintf(temp, 4096, inFormat, args);
#endif
	OutputDebugStringA(temp);
	OutputDebugStringA("\n");
}
