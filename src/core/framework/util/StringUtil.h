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
    static T stringToNumber(const std::string &str)
    {
        std::stringstream ss(str);
        T value;
        if ((ss >> value).fail())
        {
            // ERROR
            return 0;
        }
        
        return value;
    }
    
    static std::string encryptDecrypt(std::string input);
    
    static void encryptDecrypt(unsigned char* input, unsigned char* output, const long dataLength);
    
    static std::string sprintf(const char* inFormat, ...);
    
    static void log(const char* inFormat, ...);
    
private:
    // ctor, copy ctor, and assignment should be private in a Static Utility Class
    StringUtil();
    StringUtil(const StringUtil&);
    StringUtil& operator=(const StringUtil&);
};

#endif /* defined(__noctisgames__StringUtil__) */
