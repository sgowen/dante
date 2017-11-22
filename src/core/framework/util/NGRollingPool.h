//
//  NGRollingPool.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGRollingPool__
#define __noctisgames__NGRollingPool__

#include "framework/util/NGSTDUtil.h"

#include <vector>

template <class T>
class NGRollingPool
{
public:
    NGRollingPool(int poolSize) : _poolSize(poolSize), _index(0)
    {
        _objectsPool.reserve(poolSize);
        
        for (int i = 0; i < _poolSize; ++i)
        {
            _objectsPool.push_back(T());
        }
    }
    
    virtual ~NGRollingPool()
    {
        _objectsPool.clear();
    }
    
    T* newObject()
    {
        T* obj = &_objectsPool.at(_index++);
        
        if (_index >= _poolSize)
        {
            _index = 0;
        }
        
        return obj;
    }
    
    void add(T* obj)
    {
        _objectsBuffer.push_back(obj);
    }
    
    void processBuffer()
    {
        _objects.clear();
        _objects.swap(_objectsBuffer);
        _objectsBuffer.clear();
    }
    
    std::vector<T*>& getObjects()
    {
        return _objects;
    }
    
    unsigned long getBufferSize()
    {
        return _objectsBuffer.size();
    }
    
private:
    std::vector<T*> _objects;
    std::vector<T> _objectsPool;
    std::vector<T*> _objectsBuffer;
    
    int _poolSize;
    int _index;
};

#endif /* defined(__noctisgames__NGRollingPool__) */
