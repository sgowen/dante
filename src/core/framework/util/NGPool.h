//
//  NGPool.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGPool__
#define __noctisgames__NGPool__

#include <framework/util/NGExtension.h>
#include <framework/util/Vector.h>
#include <framework/util/ContainerUtil.h>

namespace NoctisGames
{
    template <typename T>
    class NGPool
    {
    public:
        NGPool()
        {
            // Empty
        }
        
        ~NGPool()
        {
            ContainerUtil::cleanUpVectorOfPointers(_objects);
        }
        
        T* obtain()
        {
            if (_objects.size() > 0)
            {
                T** object = _objects.begin();
                T* ret = *object;
                _objects.erase(0);
                
                return ret;
            }
            else
            {
                T* ret = NG_NEW(T);
                new (ret) T();
                
                return ret;
            }
        }
        
        void free(T* object)
        {
            if (!_objects.contains(object))
            {
                _objects.push_back(object);
            }
        }
        
    private:
        Vector<T*> _objects;
    };
}

#endif /* defined(__noctisgames__NGPool__) */
