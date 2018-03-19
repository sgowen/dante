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
#include <string>

struct EntityPosDef
{
    uint32_t type;
    int x;
    int y;
    int w;
    int h;
    
    EntityPosDef(uint32_t inType = 0, int inX = 0, int inY = 0, int inW = 0, int inH = 0) : type(inType), x(inX), y(inY), w(inW), h(inH) {}
};

struct EntityLayoutDef
{
    std::vector<EntityPosDef> entities;
};

struct MapDef
{
    uint32_t key;
    std::string name;
    std::string value;
    
    MapDef(uint32_t inKey, std::string inName, std::string inValue) : key(inKey), name(inName), value(inValue) {}
};

class EntityLayoutMapper
{
public:
    static EntityLayoutMapper* getInstance();
    
    static void sLayoutsDeserializerFunc(const char* data);
    static void sLayoutDeserializerFunc(const char* data);
    static const char* sLayoutSerializerFunc();
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* data);
    void loadEntityLayout(uint32_t name);
    void loadEntityLayout(std::string filePath);
    const char* save();
    void saveEntityLayout(uint32_t name, EntityLayoutDef* layout);
    void saveEntityLayout(std::string filePath, EntityLayoutDef* layout);
    EntityLayoutDef& getEntityLayoutDef();
    std::vector<MapDef>& getMaps();
    std::string getJsonConfigFilePath(uint32_t inFourCCName);
    
private:
    std::vector<MapDef> _maps;
    std::map<uint32_t, std::string> _layouts;
    EntityLayoutDef _entityLayoutDef;
    EntityLayoutDef* _layoutToSave;
    
    void loadEntityLayout(const char* data);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityLayoutMapper();
    ~EntityLayoutMapper();
    EntityLayoutMapper(const EntityLayoutMapper&);
    EntityLayoutMapper& operator=(const EntityLayoutMapper&);
};

#endif /* defined(__noctisgames__EntityLayoutMapper__) */
