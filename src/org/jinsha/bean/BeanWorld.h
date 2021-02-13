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

    Bean* getBean(oidType id);

    void findEqual(const char* propertyName,  Json::Int value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  Json::UInt value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  Json::Int64 value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  Json::UInt64 value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  bool value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  double value, std::list<Bean*>& beans);
    void findEqual(const char* propertyName,  const char* value, std::list<Bean*>& beans);
    template<typename T>
    static void findEqualCommon(const BeanWorld *world, 
        const char* propertyName,  
        const T& value, 
        std::list<Bean*>& beans);


    int getPropertyId(const char* name) const;
    const Property* getProperty(const char* name) const;
    const std::unordered_map<std::string, unsigned int>& getProperties() const 
     {return m_propertyMap_;};

    void clear();

private:

    pidType setProperty(Bean* bean,  const char* name, bool value);
    pidType setProperty(Bean* bean,  const char* name, Json::Int value);
    pidType setProperty(Bean* bean,  const char* name, Json::UInt value);
    pidType setProperty(Bean* bean,  const char* name, Json::Int64 value);
    pidType setProperty(Bean* bean,  const char* name, Json::UInt64 value);
    pidType setProperty(Bean* bean,  const char* name, double value);
    pidType setProperty(Bean* bean,  const char* name, const char* value);
    template<typename T>
    pidType doSetProperty( Bean* bean, const char* name, T value);

    pidType addProperty(const char* name);
    Json::Value removeProperty(Bean* bean, const char* name);
    void removeProperty(pidType pid);

    int addRelation(Bean* from, Bean* to, const char* propertyName);
    int removeRelation(Bean* from, Bean* to, const char* propertyName);

    oidType generateBeanId();

private:
    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 0;

    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, unsigned int> m_propertyMap_; 

friend class Bean;

};

}
}
}