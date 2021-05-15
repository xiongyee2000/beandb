
#include <assert.h>
#include <cstring>
#include <locale.h>

#include "./BeanWorld.h"
#include "./internal_common.hxx"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {


BeanWorld::BeanWorld(AbstractBeanDB *db) 
    : m_db(db)
{
    setlocale(LC_ALL, "");
}


BeanWorld::~BeanWorld()
{
    clear();
};


void BeanWorld::clear()
{
    for (auto& item : m_beans_) 
    {
        delete item.second;
        item.second = nullptr;
    }
    m_beans_.clear();

    for (auto& item : m_propertyMap_) 
    {
        delete item.second;
    }
    m_propertyMap_.clear();
}


Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateBeanId();
    m_beans_[bean->m_id_] = bean;
    return bean;
};


void BeanWorld::removeBean(oidType id)
{
    //todo: need to handle bean relations
    auto iter = m_beans_.find(id);
    if (iter != m_beans_.end())
    {
       delete iter->second;
        m_beans_.erase(iter);
    }
}


int BeanWorld::getNumOfBeans()
{
    return m_beans_.size();
}

const unordered_map<oidType, Bean*>& BeanWorld::getBeans()
{
    return m_beans_;
}


Bean* BeanWorld::getBean(oidType id)
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


Property* BeanWorld::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


Property* BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


Property* BeanWorld::defineRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


Property* BeanWorld::defineArrayRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


Property* BeanWorld::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
    if (name == nullptr) return nullptr;
    if (name[0] == 0) return nullptr;

    Property* property = nullptr;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end())
    {
        property = new Property(this, name, type, valueType, needIndex);
        m_propertyMap_[name] = property;
        //todo: current set to 0. Generate id in future...
        property->m_id_ = -1;
    }
    else
    {
        property = iter->second;
        if (property->getType() != type || property->getValueType() != valueType)
            property = nullptr; 
    }
    return property;
}


void BeanWorld::undefineProperty(const char* name)
{
    if (name == nullptr) return;
    if (name[0] == 0) return;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return;
    Property* property = iter->second;
    delete property;
    m_propertyMap_.erase(iter);
}


const Property* BeanWorld::getProperty(const char* name) const
{
    if (name == nullptr) return nullptr;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end())
        return nullptr;
    else
        return iter->second;
}


Property* BeanWorld::getProperty(const char* name)
{
    return const_cast<Property*>(((const BeanWorld*)this)->getProperty(name));
}


int BeanWorld::loadAll()
{
    if (m_db != nullptr) {
        return m_db->loadAll();
    } else {
        return 0;
    }
}


int BeanWorld::unloadAll()
{
    //todo
    return 0;
}


int BeanWorld::loadProperties()
{
    if (m_db != nullptr) {
        return m_db->loadAll();
    } else {
        return 0;
    }
}


Bean* BeanWorld::loadBean(oidType id)
{
    if (m_db != nullptr) {
        return m_db->loadBean(id);
    } else {
        return nullptr;
    }
}


int BeanWorld::unloadBean(oidType id)
{
    //todo
    return 0;
}


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
