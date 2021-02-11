
#include <assert.h>
#include <cstring>
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

Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateBeanId();
    m_beans_[bean->m_id_] = bean;
    return bean;
};

void BeanWorld::removeBean(Bean* bean)
{
    //todo
}

int BeanWorld::getNumOfBeans()
{
    return m_beans_.size();
}

const unordered_map<oidType, Bean*>& BeanWorld::getBeans()
{
    return m_beans_;
}

Bean* BeanWorld::findBean(oidType id)
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

Bean* BeanWorld::findBean(pidType pid,  const Json::Value& value)
{
    if (pid <0 || pid >= m_properties_.size()) return nullptr;

    // auto& valueMap = m_pvalues_[pid];
    // auto iter = valueMap.find(value);
    
    return nullptr;
}

void BeanWorld::removeBean(oidType id)
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
}


template<typename T>
int BeanWorld::setPropertyBase( Bean* bean, const char* name, T value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;

    pidType  pid = getPropertyIndex(name);
    Property* property = nullptr;

    if (pid == -1)
    {
        pid = addProperty(name);
        property = m_properties_[pid];
    }
    else
    {
        property = m_properties_[pid];
        if (bean->isMember(name))
        {
            property->removeIndex(bean, (*bean)[name]);
        } 
        else
        {
            property->refCount_++;
        }
    }
    
    //set value for json object
    auto& v = (*bean)[name];
    v = value;
    property->addIndex(bean, value);

    return pid;
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::Int value)
{
    return setPropertyBase<Json::Int>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt value)
{
    return setPropertyBase<Json::UInt>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::Int64 value)
{
    return setPropertyBase<Json::Int64>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt64 value)
{
    return setPropertyBase<Json::UInt64>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, bool value)
{
    return setPropertyBase<bool>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, double value)
{
    return setPropertyBase<double>(bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, const char* value)
{
    return setPropertyBase<const char*>(bean, name, value);
}

Json::Value BeanWorld::removeProperty(Bean* bean, const char* name)
{
    Json::Value v;
    if (name == nullptr) return v;
    if (*name == 0) return v;

    //remove member of json object first
    v = bean->jsonValue_.removeMember(name);
    removeProperty(name);
    return v;
}

pidType BeanWorld::addProperty(const char* name)
{
    int index = getPropertyIndex(name);
    if (index < 0)
    {//no such property
        Property* property = new Property(name);
        property->refCount_ = 1;
        m_properties_.push_back(property);
        m_propertyMap[name] = m_properties_.size() - 1;
        return m_properties_.size() - 1;
    }
    else
    {
        return -1;
    }
}

void BeanWorld::removeProperty(const char* name)
{
    if (name == nullptr) return;
    if (name[0] == 0) return;
    auto iter = m_propertyMap.find(name);
    if (iter == m_propertyMap.end()) return; //no such property
    
    auto index = iter->second;
    auto property = m_properties_[index];
    if (property->refCount_ < 1)
    {
        //do nothing
    }
    else if (property->refCount_ == 1)
    { //need to remove this property
        //todo
        delete m_properties_[index];
        m_properties_[index] = nullptr;
        m_propertyMap.erase(iter);
    }
    else
    {
        property->refCount_--;
    }
}

int BeanWorld::addRelation(Bean* from, Bean* to, const char* propertyName)
{
    if (propertyName == nullptr) return -1;
    if (propertyName[0] == 0) return -1;

    pidType  pid = addProperty(propertyName);
    (*from)[propertyName] = to->getId();
}

int BeanWorld::removeRelation(Bean* from, Bean* to, const char* propertyName)
{
    if (propertyName == nullptr) return -1;
    if (propertyName[0] == 0) return -1;

    from->removeMember(propertyName);

    pidType  pid = getPropertyIndex(propertyName);
        //todo
        // m_propertyIndexTypes_[pid] = Json::objectValue;
}

oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};

}
}
}
