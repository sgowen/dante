//
//  NGRTTI.h
//  noctisgames
//
//  Created by Stephen Gowen on 11/4/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGRTTI__
#define __noctisgames__NGRTTI__

#include <string>

class NGRTTI
{
public:
    NGRTTI(const std::string& className);
    
    NGRTTI(const std::string& className, const NGRTTI& baseNGRTTI);

    const std::string& getClassName() const;
    
    bool isExactly(const NGRTTI& rtti) const;
    
    bool derivesFrom(const NGRTTI& rtti) const;

private:
    // Prevent copying
    NGRTTI(const NGRTTI& obj);
    NGRTTI& operator=(const NGRTTI& obj);

    const std::string _className;
    const NGRTTI *_pBaseNGRTTI;
};

#define NGRTTI_DECL \
public: \
    static const NGRTTI rtti; \
    virtual const NGRTTI& getRTTI();

#define NGRTTI_IMPL_NOPARENT(name) \
    const NGRTTI name::rtti(#name); \
    const NGRTTI& name::getRTTI() { return rtti; }

#define NGRTTI_IMPL(name,parent) \
    const NGRTTI name::rtti(#name, parent::rtti); \
    const NGRTTI& name::getRTTI() { return rtti; }

#endif /* defined(__noctisgames__NGRTTI__) */
