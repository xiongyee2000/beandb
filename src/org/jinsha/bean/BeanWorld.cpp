
#include <assert.h>

#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

BeanWorld::BeanWorld()
{

}

BeanWorld::~BeanWorld()
{
    clear();
};

int BeanWorld::getNumOfBeans()
{
    return m_beans_.size();
}

Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateBeanId();
    m_beans_[bean->m_id_] = bean;
    return bean;
};

Bean* BeanWorld::findBean(otype id)
{
    auto iter = m_beans_.find(id);
    if (iter == m_beans_.end())
    {
        return nullptr;
    }
    else
    {
        return iter->second;
    }
}
void BeanWorld::removeBean(otype id)
{
    auto iter = m_beans_.find(id);
    if (iter != m_beans_.end())
    {
        auto bean = iter->second;
        // auto memberNames = bean->getMemberNames();
        // for (auto& memberName : memberNames) 
        // {
        //     //todo
        // }
        delete bean;
        m_beans_.erase(iter);
    }
}

int BeanWorld::getPropertyIndex(const char* name) const
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap.find(name);
    if (iter == m_propertyMap.end()) return -1; //no such property
    return (int)(iter->second);
}

// const  Property* BeanWorld::getProperty(const char* name) const
// {
//     int index = getPropertyIndex(name);
//     if (index < 0) 
//     {
//         return nullptr;
//     }
//     else
//     {
//         return m_properties_[index];
//     }
// }

void BeanWorld::clear()
{
    for (auto item : m_beans_) 
    {
        delete item.second;
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

int BeanWorld::setProperty(Bean* bean, const char* name, const Json::Value& value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;

    //set value for json object first
     (*bean)[name] = value;
     addProperty(name);

    return 0;
}

void BeanWorld::removeProperty(Bean* bean, const char* name)
{
    if (name == nullptr) return;
    if (*name == 0) return;

    //remove member of json object first
    (*bean).removeMember(name);
    removeProperty(name);
}

void BeanWorld::addProperty(const char* name)
{
    int index = getPropertyIndex(name);
    if (index < 0) 
    {//no such property
        Property* property = new Property(name);
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

otype BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};

}
}
}