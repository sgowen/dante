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

class StringUtil
{
public:
    static std::string encryptDecrypt(std::string input)
    {
        char key[3] = {'N', 'G', 'S'}; // Any chars will work, in an array of any size
        std::string output = input;
        
        for (unsigned int i = 0; i < input.size(); ++i)
        {
            output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];
        }
        
        return output;
    }
    
    template<typename T>
    static std::string toString(const T& n)
    {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    }
    
    static int stringToInt(const std::string &str)
    {
        std::stringstream ss(str);
        int num;
        if ((ss >> num).fail())
        {
            // ERROR
            return 0;
        }
        
        return num;
    }
};

#endif /* defined(__noctisgames__StringUtil__) */
