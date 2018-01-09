//
//  EntityLayout.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityLayout__
#define __noctisgames__EntityLayout__

#include <map>
#include <string>

class b2World;
class Entity;
class EntityDef;
class EntityController;

typedef EntityController* (*EntityControllerCreationFunc)(Entity* inEntity);

class EntityLayout
{
public:
    static EntityLayout* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* jsonFile, bool isBundled, bool useEncryption = false);
    void initWithJson(const char* json);
    
    Entity* createEntity(uint32_t inFourCCName, b2World& world, bool isServer);
    
    void registerFunction(std::string name, EntityControllerCreationFunc inCreationFunction);
    EntityController* createEntityController(std::string name, Entity* inEntity);
    
    std::map<uint32_t, EntityDef*>& getEntityDescriptors();
    
private:
    std::map<uint32_t, EntityDef*> _entityDescriptors;
    std::map<std::string, EntityControllerCreationFunc> _nameToEntityControllerCreationFunctionMap;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityLayout();
    ~EntityLayout();
    EntityLayout(const EntityLayout&);
    EntityLayout& operator=(const EntityLayout&);
};

#endif /* defined(__noctisgames__EntityLayout__) */
