
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
    m_propertiesRefCounts_.clear();
    // m_pvalues_.clear();
}

bool BeanWorld::isPropertyIndexed(pidType pid)
{
    //todo
    return false;
}

template<typename T>
static int setPropertyBase(BeanWorld* world, Bean* bean, const char* name, T value)
{
   if (name == nullptr) return -1;
    if (name[0] == 0) return -1;

    //set value for json object
    auto& v = (*bean)[name];
    v = value;
    pidType  pid = world->addProperty(name);

    if (world->isPropertyIndexed(pid))
    {
        world->m_propertyIndexTypes_[pid] = v.type();
        if (typeid(value) == typeid((const char*)nullptr))
        {
            world->m_propertyIndices_[pid].insert (std::pair<uint64_t, Bean*>((uint64_t)v.asCString(), bean));
        }
        else if (typeid(value) == typeid(true))
        {
            world->m_propertyIndices_[pid].insert (std::pair<uint64_t, Bean*>((uint64_t)(v.asBool() ? 1 : 0), bean));
        }
        else if ((typeid(value) == typeid(0.1d)))
        {
            world->m_propertyIndices_[pid].insert (std::pair<uint64_t, Bean*>((uint64_t)value, bean));
        }
        else
        {
           world-> m_propertyIndices_[pid].insert (std::pair<uint64_t, Bean*>((uint64_t)value, bean));
        }
    }

    return pid;
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::Int value)
{
    return setPropertyBase<Json::Int>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt value)
{
    return setPropertyBase<Json::UInt>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::Int64 value)
{
    return setPropertyBase<Json::Int64>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt64 value)
{
    return setPropertyBase<Json::UInt64>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, bool value)
{
    return setPropertyBase<bool>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, double value)
{
    return setPropertyBase<double>(this, bean, name, value);
}

int BeanWorld::setProperty(Bean* bean, const char* name, const char* value)
{
    return setPropertyBase<const char*>(this, bean, name, value);
}

void BeanWorld::removeProperty(Bean* bean, const char* name)
{
    if (name == nullptr) return;
    if (*name == 0) return;

    //remove member of json object first
    (*bean).removeMember(name);
    removeProperty(name);
}

pidType BeanWorld::addProperty(const char* name)
{
    int index = getPropertyIndex(name);
    if (index < 0)
    {//no such property
        Property* property = new Property(name);
        m_properties_.push_back(property);
        m_propertiesRefCounts_.push_back(1);
        m_propertyMap[name] = m_properties_.size() - 1;
        return m_properties_.size() - 1;
    }
    else
    {
        m_propertiesRefCounts_[index]++;
        return index;
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

oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};

}
}
}
