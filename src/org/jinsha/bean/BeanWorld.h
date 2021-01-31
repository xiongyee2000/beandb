#pragma once

// #include <memory>
#include <vector>
#include <list>
#include <unordered_map>

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

    int getNumOfBeans();
    Bean *createBean();
    void removeBean(otype id);
    Bean* findBean(otype id);

    // const Property* getProperty(const char* name) const;
    int getPropertyIndex(const char* name) const;
    const std::unordered_map<std::string, unsigned int>& getProperties() const {return m_propertyMap;};

    void clear();

private:
    int setProperty(Bean* bean,  const char* name, const Json::Value& value);
    void removeProperty(Bean* bean, const char* name);
    void addProperty(const char* name);
    void removeProperty(const char* name);

    otype generateBeanId();

    std::unordered_map<otype, Bean*> m_beans_;
    otype m_maxBeanId_ = 0;
    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, unsigned int> m_propertyMap; 
    std::vector<unsigned int> m_propertiesRefCounts_;

friend class Bean;
};

}
}
}