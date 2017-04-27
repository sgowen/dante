//
//  NGSTDUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSTDUtil__
#define __noctisgames__NGSTDUtil__

#include <vector>
#include <memory>
#include <map>

class NGSTDUtil
{
public:
    template<typename T>
    static void cleanUpVectorOfPointers(std::vector<T*>& items)
    {
        for (typename std::vector<T*>::iterator i = items.begin(); i != items.end(); )
        {
            delete *i;
            
            i = items.erase(i);
        }
    }
    
    template<typename T>
    static void cleanUpVectorOfUniquePointers(std::vector<std::unique_ptr<T>>& items)
    {
        for (typename std::vector<std::unique_ptr<T>>::iterator i = items.begin(); i != items.end(); )
        {
            (*i).reset();
            
            i = items.erase(i);
        }
    }
    
    template<typename K, typename T>
    static void cleanUpMapOfPointerValues(std::map<K, T*>& items)
    {
        for(typename std::map<K, T*>::iterator i = items.begin(); i != items.end(); )
        {
            delete i->second;
            
            i = items.erase(i);
        }
    }
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGSTDUtil();
    NGSTDUtil(const NGSTDUtil&);
    NGSTDUtil& operator=(const NGSTDUtil&);
};

#endif /* defined(__noctisgames__NGSTDUtil__) */
