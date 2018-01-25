//
//  EntityLayoutMapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityLayoutMapper__
#define __noctisgames__EntityLayoutMapper__

#include <map>
#include <vector>

struct EntityPosDef
{
    uint32_t type;
    float x;
    float y;
    
    EntityPosDef(uint32_t inType = 0, float inX = 0, float inY = 0) : type(inType), x(inX), y(inY) {}
};

struct EntityLayoutDef
{
    std::vector<EntityPosDef> layers;
    std::vector<EntityPosDef> staticEntities;
    std::vector<EntityPosDef> dynamicEntities;
};

class EntityLayoutMapper
{
public:
    static EntityLayoutMapper* getInstance();
    
    static void sLayoutsDeserializerFunc(const char* data);
    static void sLayoutDeserializerFunc(const char* data);
    static const char* sLayoutSerializerFunc();
    
    void initWithJsonFile(const char* jsonFile, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* data);
    void loadEntityLayout(uint32_t name);
    const char* save();
    void saveEntityLayout(uint32_t name, EntityLayoutDef* layout);
    EntityLayoutDef& getEntityLayoutDef();
    std::string getJsonConfigFilePath(uint32_t inFourCCName);
    
private:
    std::map<uint32_t, std::string> _layouts;
    EntityLayoutDef _entityLayoutDef;
    EntityLayoutDef* _layoutToSave;
    
    void loadEntityLayout(const char* data);
    std::string adjustPath(const char* path);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityLayoutMapper();
    ~EntityLayoutMapper();
    EntityLayoutMapper(const EntityLayoutMapper&);
    EntityLayoutMapper& operator=(const EntityLayoutMapper&);
};

#endif /* defined(__noctisgames__EntityLayoutMapper__) */
