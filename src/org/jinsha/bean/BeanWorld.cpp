
#include <assert.h>
#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

BeanWorld::~BeanWorld()
{
    clear();
};

void BeanWorld::clear()
{
    for (auto item : m_beans_) 
    {
        delete item;
    }
    m_beans_.clear();

    for (auto item : m_properties_) 
    {
        delete item;
    }
    m_properties_.clear();
    
    m_propertyMap.clear();
    m_propertiesRefCounts_.clear();
}

Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateObjectId();
    m_beans_.push_back(bean);
    return bean;
};

int BeanWorld::getPropertyIndex(const char* name) const
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap.find(name);
    if (iter == m_propertyMap.end()) return -1; //no such property
    return (int)(iter->second);
}

const  Property* BeanWorld::getProperty(const char* name) const
{
    int index = getPropertyIndex(name);
    if (index < 0) 
    {
        return nullptr;
    }
    else
    {
        return m_properties_[index];
    }
}

void BeanWorld::addProperty(const char* name)
{
    int index = getPropertyIndex(name);
    if (index < 0) 
    {//no such property
        auto property = new Property(name);
        m_properties_.push_back(property);
        m_propertiesRefCounts_.push_back(1);
        m_propertyMap[name] = m_properties_.size() - 1;
    }
    else
    {
        m_propertiesRefCounts_[index]++;
    }
}

void BeanWorld::removeProperty(const char* name)
{
    if (name == nullptr) return;
    if (name[0] == 0) return;
    auto iter = m_propertyMap.find(name);
    if (iter == m_propertyMap.end()) return; //no such property
    
    auto index = iter->second;
    if (m_propertiesRefCounts_[index] < 1)
    {
        //do nothing
    }
    else if (m_propertiesRefCounts_[index] == 1)
    { //need to remove this property
        delete m_properties_[index];
        m_properties_[index] = nullptr;
        m_propertiesRefCounts_[index] = 0;
        m_propertyMap.erase(iter);
    }
    else
    {
        m_propertiesRefCounts_[index]--;
    }
}

otype BeanWorld::generateObjectId()
{
    //todo
    return 0;
};

}
}
}