//
//  RTTI.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/4/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RTTI__
#define __noctisgames__RTTI__

#include <string>

class RTTI
{
public:
    RTTI(const std::string & className);
    RTTI(const std::string & className, const RTTI & baseRTTI);

    const std::string & getClassName() const;
    
    bool isExactly(const RTTI & rtti) const;
    
    bool derivesFrom(const RTTI & rtti) const;

private:
    // Prevent copying
    RTTI(const RTTI & obj);
    RTTI & operator=(const RTTI & obj);

    const std::string m_className;
    const RTTI *m_pBaseRTTI;
};

#define RTTI_DECL \
    public: \
        virtual const RTTI & getRTTI() { return rtti; } \
        static const RTTI rtti;

#define RTTI_IMPL_NOPARENT(name) \
    const RTTI name::rtti(#name);

#define RTTI_IMPL(name,parent) \
    const RTTI name::rtti(#name, parent::rtti);

#endif /* defined(__noctisgames__RTTI__) */
