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
    BeanWorld() {};
    virtual ~BeanWorld();

    void clear();

    const std::vector<Property*> & getProperties() const {return m_properties_;};

    const Property* getProperty(const char* name) const;
    int getPropertyIndex(const char* name) const;

    Bean *createBean();

private:
    void addProperty(const char* name);
    void removeProperty(const char* name);

    otype generateObjectId();

    std::list<Bean*> m_beans_;
    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, size_t> m_propertyMap; 
    std::vector<unsigned int> m_propertiesRefCounts_;

friend class Bean;
};

}
}
}