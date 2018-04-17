//
//  ContainerUtil.h
//  noctisgames
//
//  Created by Stephen Gowen on 11/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ContainerUtil__
#define __noctisgames__ContainerUtil__

#include <framework/util/NGExtension.h>
#include <framework/util/Vector.h>

namespace NoctisGames
{
    class ContainerUtil
    {
    public:
        template<typename T>
        static void cleanUpVectorOfPointers(Vector<T*>& items)
        {
            for (size_t i = 0; i < items.size(); )
            {
                T* item = items[i];
                
                NG_DESTROY(T, item);
                
                items.erase(i);
            }
        }
        
    private:
        // ctor, copy ctor, and assignment should be private in a Singleton
        ContainerUtil();
        ContainerUtil(const ContainerUtil&);
        ContainerUtil& operator=(const ContainerUtil&);
    };
}

#endif /* defined(__noctisgames__ContainerUtil__) */
