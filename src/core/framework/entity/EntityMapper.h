//
//  EntityMapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityMapper__
#define __noctisgames__EntityMapper__

#include <map>
#include <vector>
#include <string>

class Entity;
struct EntityDef;
class EntityController;

typedef EntityController* (*EntityControllerCreationFunc)(Entity* inEntity);

class EntityMapper
{
public:
    static EntityMapper* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* jsonFile, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* json);
    Entity* createEntity(uint32_t inFourCCName, int x = 0, int y = 0, bool isServer = false);
    Entity* createEntityFromDef(EntityDef* entityDef, int x = 0, int y = 0, bool isServer = false);
    void registerFunction(std::string name, EntityControllerCreationFunc inCreationFunction);
    EntityController* createEntityController(std::string name, Entity* inEntity);
    const std::vector<EntityDef*>& getEntityDescriptors();
    const std::map<std::string, EntityControllerCreationFunc>& getEntityControllerMap();
    
private:
    std::vector<EntityDef*> _entityDescriptors;
    std::map<uint32_t, EntityDef*> _entityDescriptorsMap;
    std::map<std::string, EntityControllerCreationFunc> _nameToEntityControllerCreationFunctionMap;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityMapper();
    ~EntityMapper();
    EntityMapper(const EntityMapper&);
    EntityMapper& operator=(const EntityMapper&);
};

#endif /* defined(__noctisgames__EntityMapper__) */
