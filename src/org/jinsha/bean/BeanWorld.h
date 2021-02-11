#pragma once

// #include <memory>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanWorld
{
public:
    BeanWorld();
    virtual ~BeanWorld();

    Bean *createBean();
    void removeBean(oidType id);

    int getNumOfBeans();
    const std::unordered_map<oidType, Bean*>& getBeans();

    Bean* findBean(oidType id);
    Bean* findBean(pidType pid,  const Json::Value& value);

    // const Property* getProperty(const char* name) const;
    int getPropertyIndex(const char* name) const;
    const std::unordered_map<std::string, unsigned int>& getProperties() const 
     {return m_propertyMap;};

    void removeBean(Bean* bean);

    void clear();

private:
    // int setProperty(Bean* bean,  const char* name, const Json::Value& value);
    int setProperty(Bean* bean,  const char* name, bool value);
    int setProperty(Bean* bean,  const char* name, Json::Int value);
    int setProperty(Bean* bean,  const char* name, Json::UInt value);
    int setProperty(Bean* bean,  const char* name, Json::Int64 value);
    int setProperty(Bean* bean,  const char* name, Json::UInt64 value);
    int setProperty(Bean* bean,  const char* name, double value);
    int setProperty(Bean* bean,  const char* name, const char* value);
    Json::Value removeProperty(Bean* bean, const char* name);
    template<typename T>
    int setPropertyBase( Bean* bean, const char* name, T value);

    pidType addProperty(const char* name);
    void removeProperty(const char* name);

    int addRelation(Bean* from, Bean* to, const char* propertyName);
    int removeRelation(Bean* from, Bean* to, const char* propertyName);

    oidType generateBeanId();

    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 0;

    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, unsigned int> m_propertyMap; 

friend class Bean;

};

}
}
}