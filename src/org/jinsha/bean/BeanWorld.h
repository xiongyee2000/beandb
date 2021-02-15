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

    void clear();

    Bean *createBean();
    void removeBean(oidType id);

    int getNumOfBeans();
    const std::unordered_map<oidType, Bean*>& getBeans();

    Bean* getBean(oidType id);

    int getPropertyId(const char* name) const;
    const Property* getProperty(pidType pid) const;
    const Property* getProperty(const char* name) const;
    const std::unordered_map<std::string, unsigned int>& getProperties() const 
     {return m_propertyMap_;};

    void findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

private:
    pidType setProperty( Bean* bean, const char* name, const Json::Value&  value);
    pidType addProperty(const char* name);
    Json::Value removeProperty(Bean* bean, const char* name);
    void removeProperty(pidType pid);

    void findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

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