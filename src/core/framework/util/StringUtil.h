//
//  StringUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__StringUtil__
#define __noctisgames__StringUtil__

#include <string>
#include <sstream>
#include <cstdarg>

// OUT_Z_ARRAY indicates an output array that will be null-terminated.
#if _MSC_VER >= 1600
    // Include the annotation header file.
    #include <sal.h>
    #if _MSC_VER >= 1700
        // VS 2012+
        #define OUT_Z_ARRAY _Post_z_
    #else
        // VS 2010
        #define OUT_Z_ARRAY _Deref_post_z_
    #endif
#else
    // gcc, clang, old versions of VS
    #define OUT_Z_ARRAY
#endif

#define LOG(...) StringUtil::log(__VA_ARGS__);

class StringUtil
{
public:
    template<typename T>
    static std::string toString(const T& n)
    {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    }

    template<typename T>
    static T stringToNumber(const std::string &str, T defVal = 0)
    {
        std::stringstream ss(str);
        T value;
        if ((ss >> value).fail())
        {
            return defVal;
        }

        return value;
    }
    
    static bool stringToBool(const std::string &str, bool defVal = false)
    {
        if (str.length() > 0)
        {
            return str == "true";
        }
        
        return defVal;
    }

    template<size_t maxLenInChars>
    static void sprintf_safe(OUT_Z_ARRAY char (&pDest)[maxLenInChars], const char *pFormat, ...)
    {
        va_list params;
        va_start(params, pFormat);

#ifdef POSIX
        vsnprintf(pDest, maxLenInChars, pFormat, params);
#else
        _vsnprintf(pDest, maxLenInChars, pFormat, params);
#endif

        pDest[maxLenInChars - 1] = '\0';

        va_end(params);
    }

    static std::string encryptDecrypt(std::string input);

    static void encryptDecrypt(unsigned char* input, unsigned char* output, const long dataLength);

    static std::string format(const char* inFormat, ...);

    static void log(const char* inFormat, ...);

private:
    // ctor, copy ctor, and assignment should be private in a Static Utility Class
    StringUtil();
    StringUtil(const StringUtil&);
    StringUtil& operator=(const StringUtil&);
};

#endif /* defined(__noctisgames__StringUtil__) */
