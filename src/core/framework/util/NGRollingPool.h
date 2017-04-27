//
//  NGRollingPool.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGRollingPool__
#define __noctisgames__NGRollingPool__

#include "NGSTDUtil.h"

#include <vector>

template <class T>
class NGRollingPool
{
public:
    NGRollingPool(int poolSize) : m_iPoolSize(poolSize), m_iIndex(0)
    {
        for (int i = 0; i < m_iPoolSize; ++i)
        {
            m_objectsPool.push_back(new T());
        }
    }
    
    virtual ~NGRollingPool()
    {
        NGSTDUtil::cleanUpVectorOfPointers(m_objectsPool);
    }
    
    T* newObject()
    {
        T* obj = m_objectsPool.at(m_iIndex++);
        
        if (m_iIndex >= m_iPoolSize)
        {
            m_iIndex = 0;
        }
        
        return obj;
    }
    
    void add(T* obj)
    {
        m_objectsBuffer.push_back(obj);
    }
    
    void processBuffer()
    {
        m_objects.clear();
        m_objects.swap(m_objectsBuffer);
        m_objectsBuffer.clear();
    }
    
    std::vector<T*>& getObjects()
    {
        return m_objects;
    }
    
    unsigned long getBufferSize()
    {
        return m_objectsBuffer.size();
    }
    
private:
    std::vector<T*> m_objects;
    std::vector<T*> m_objectsPool;
    std::vector<T*> m_objectsBuffer;
    
    int m_iPoolSize;
    int m_iIndex;
};

#endif /* defined(__noctisgames__NGRollingPool__) */
