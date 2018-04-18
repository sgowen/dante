//
//  EntityMapper.h
//  noctisgames
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
struct EntityInstanceDef;
class EntityController;
class EntityNetworkController;

typedef EntityController* (*EntityControllerCreationFunc)(Entity* e);
typedef EntityNetworkController* (*EntityNetworkControllerCreationFunc)(Entity* e, bool isServer);

class EntityMapper
{
public:
    static EntityMapper* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* json);
    Entity* createEntity(EntityInstanceDef* inEntityInstanceDef, bool isServer);
    Entity* createEntityFromDef(EntityDef* inEntityDef, EntityInstanceDef* inEntityInstanceDef, bool isServer);
    EntityDef* getEntityDef(uint32_t inFourCCName);
    void registerFunction(std::string name, EntityControllerCreationFunc func);
    void registerFunction(std::string name, EntityNetworkControllerCreationFunc func);
    EntityController* createEntityController(EntityDef& ed, Entity* e);
    EntityNetworkController* createEntityNetworkController(EntityDef& ed, Entity* e);
    const std::vector<EntityDef*>& getEntityDescriptors();
    const std::map<std::string, EntityControllerCreationFunc>& getEntityControllerMap();
    const std::map<std::string, EntityNetworkControllerCreationFunc>& getEntityNetworkControllerMap();
    
private:
    std::map<uint32_t, EntityDef*> _entityDescriptorsMap;
    std::vector<EntityDef*> _entityDescriptors;
    std::map<std::string, EntityControllerCreationFunc> _entityControllerCreationFunctionMap;
    std::map<std::string, EntityNetworkControllerCreationFunc> _entityNetworkControllerCreationFunctionMap;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityMapper();
    ~EntityMapper();
    EntityMapper(const EntityMapper&);
    EntityMapper& operator=(const EntityMapper&);
};

#endif /* defined(__noctisgames__EntityMapper__) */
