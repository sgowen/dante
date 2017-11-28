//
//  Vector.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Vector__
#define __noctisgames__Vector__

#include <framework/util/NGExtension.h>

#include <stdlib.h>
#include <memory>
#include <assert.h>

namespace NoctisGames
{
    template <typename T>
    class Vector
    {
    public:
        Vector() : _size(0), _capacity(0), _buffer(NULL)
        {
            // Empty
        }
        
        Vector(const Vector& inVector) : _size(inVector._size), _capacity(inVector._capacity), _buffer(NULL)
        {
            if (_capacity > 0)
            {
                _buffer = allocate(_capacity);
                for (size_t i = 0; i < _size; ++i)
                {
                    construct(_buffer + i, inVector._buffer[i]);
                }
            }
        }
        
        Vector& operator=(Vector& inVector)
        {
            if (this != &inVector)
            {
                clear();
                deallocate(_buffer);
                
                _size = inVector._size;
                _capacity = inVector._capacity;
                
                if (_capacity > 0)
                {
                    _buffer = allocate(_capacity);
                    for (size_t i = 0; i < _size; ++i)
                    {
                        construct(_buffer + i, inVector._buffer[i]);
                    }
                }
            }
            
            return *this;
        }
        
        ~Vector()
        {
            clear();
            deallocate(_buffer);
        }
        
        bool contains(const T& inValue)
        {
            for (size_t i = 0; i < _size; ++i)
            {
                if (_buffer[i] == inValue)
                {
                    return true;
                }
            }
            
            return false;
        }
        
        int indexOf(const T& inValue)
        {
            for (size_t i = 0; i < _size; ++i)
            {
                if (_buffer[i] == inValue)
                {
                    return static_cast<int>(i);
                }
            }
            
            return -1;
        }
        
        void push_back(const T& inValue)
        {
            if (_size == _capacity)
            {
                reserve();
            }
            
            construct(_buffer + _size++, inValue);
        }
        
        void insert(size_t inIndex, const T& inValue)
        {
            assert(inIndex < _size);
            
            if (_size == _capacity)
            {
                reserve();
            }
            
            for (size_t i = ++_size - 1; i > inIndex; --i)
            {
                construct(_buffer + i, _buffer[i - 1]);
                destroy(_buffer + (i - 1));
            }
            
            construct(_buffer + inIndex, inValue);
        }
        
        void erase(size_t inIndex)
        {
            assert(inIndex < _size);
            
            --_size;
            
            if (inIndex != _size)
            {
                for (size_t i = inIndex; i < _size; ++i)
                {
                    std::swap(_buffer[i], _buffer[i + 1]);
                }
            }
            
            destroy(_buffer + _size);
        }
        
        void clear()
        {
            for (size_t i = 0; i < _size; ++i)
            {
                destroy(_buffer + (_size - 1 - i));
            }
            
            _size = 0;
        }
        
        size_t size() const
        {
            return _size;
        }
        
        T& operator[](size_t inIndex)
        {
            assert(inIndex < _size);
            
            return _buffer[inIndex];
        }
        
        void reserve(size_t inCapacity = 0)
        {
            size_t newCapacity = inCapacity > 0 ? inCapacity : _capacity > 0 ? _capacity * 2 : 1;
            if (newCapacity > _capacity)
            {
                _buffer = NG_REALLOC(_buffer, T, newCapacity);
                _capacity = newCapacity;
            }
        }
        
        T* begin()
        {
            return &_buffer[0];
        }
        
        T* end()
        {
            return &_buffer[_size];
        }
        
        friend bool operator==(Vector<T>& lhs, Vector<T>& rhs)
        {
            if (lhs.size() != rhs.size())
            {
                return false;
            }
            
            for (int i = 0, n = static_cast<int>(lhs.size()); i < n; ++i)
            {
                if (lhs[i] != rhs[i])
                {
                    return false;
                }
            }
            
            return true;
        }
        
        friend bool operator!=(Vector<T>& lhs, Vector<T>& rhs)
        {
            return !(lhs == rhs);
        }
        
    private:
        size_t _size;
        size_t _capacity;
        T* _buffer;
        
        T* allocate(size_t n)
        {
            assert(n > 0);
            
            T* ptr = NG_MALLOC(T, n);
            
            assert(ptr);
            
            return ptr;
        }
        
        void deallocate(T* buffer)
        {
            NG_FREE(buffer);
        }
        
        void construct(T* buffer, const T& val)
        {
            /// This is a placement new operator
            /// which basically means we are contructing a new object
            /// using pre-allocated memory
            new (buffer) T(val);
        }
        
        void destroy(T* buffer)
        {
            buffer->~T();
        }
    };
}

#endif /* defined(__noctisgames__Vector__) */
