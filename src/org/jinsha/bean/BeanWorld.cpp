
#include <assert.h>
#include <cstring>
#include <locale.h>

#include "./BeanWorld.h"
#include "./internal_common.hxx"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {


BeanWorld::BeanWorld(AbstractBeanDB& db) 
    : m_db(&db)
{
    setlocale(LC_ALL, "");
    m_db->m_world_ = this;
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
   m_properties_loaded_ = false;
}


Bean* BeanWorld::createBean()
{
    int err = 0;
    oidType id = 0;
    Bean* bean = nullptr;
    err  = m_db->createBean_(id);
    if (err) {
        elog("Failed to create bean in database. (err=%d) \n", err);
    } else {
        bean = new Bean(id, this);
        assert(bean);
        m_beans_[id] = bean;
    } 
    return bean;
};


void BeanWorld::removeBean(oidType id)
{
    auto iter = m_beans_.find(id);
    if (iter != m_beans_.end())
    {
        //handle relations: remove relation from subject that has
        Bean* bean = iter->second;
        //relation to this bean (as object)
        Bean* subject = nullptr;
        Property* property = nullptr;
        Json::Value* value = nullptr;
        auto iter2 = bean->m_subjectMap_.begin();
        while (iter2 != bean->m_subjectMap_.end())
        {
            subject = getBean(iter2->first);
            if (subject == nullptr) {
                iter2 = bean->m_subjectMap_.erase(iter2);
                continue;
            }
            property = iter2->second;
            if (property->getType() == Property::RelationType)  {
                //use doRemoveProperty(property, true) to keep
                //m_subjectMap_ unchanged
                subject->doRemoveProperty(property, true, true);
            } else if (property->getType() == Property::ArrayRelationType) {
                value = subject->getMemberPtr(property);
                if (value == nullptr) {
                    iter2++;
                    continue; //shall not be null
                }
                size_t size = value->size();
                //todo: O(n*n) complexity! How to improve performance?
                for (Json::ArrayIndex i = size; i > 0; i--)
                    if (subject->getRelationBeanId(property, i - 1) == bean->m_id_) {
                        subject->doRemoveProperty(property, i - 1, true, true); 
                    }
            }
            iter2++;
        }

        iter->second->unload();
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


Bean* BeanWorld::getBean(oidType id,  bool loadFromDB)
{
    int err = 0;
    Bean* bean = nullptr;
    auto iter = m_beans_.find(id);
    if (iter == m_beans_.end())
    { //bean not found in this world
        if (!loadFromDB) return nullptr;
        //try to load it from db
        bean = new Bean(id, this);
        err = bean->load();
        if (err) { //no such bean in db
            delete bean;
            bean = nullptr;
        } else {
            m_beans_[id] = bean;
        }
    } else {
        bean = iter->second;
    }

    return bean;
}


int BeanWorld::deleteBean(Bean* bean)
{
    if (bean == nullptr) return 0;
    
    int err = 0;
    err = m_db->deleteBean_(bean);
    if (!err) {
        removeBean(bean->getId());
    }
    
    return err;
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


Property* BeanWorld::definePropertyCommon_(const char* name, 
    Property::Type type, 
    Property::ValueType valueType, 
    bool needIndex)
{
    if (name == nullptr) return nullptr;
    if (name[0] == 0) return nullptr;

    int err = 0;
    pidType pid = 0;
    Property* property = nullptr;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end())
    {
        bool delayLoad = false;
        err = m_db ->defineProperty_(name, type, valueType, needIndex, pid, delayLoad);
        if (err) {
            elog("Failed to define property %s in database.", name);
            return nullptr;
        } else {
            property = new Property(this, name, pid, type, valueType, delayLoad, needIndex);
            m_propertyMap_[name] = property;
            property->m_id_ = pid;
        }
    }
    else
    {
        property = iter->second;
        if (property->getType() != type || 
            property->getValueType() != valueType ||
            property->indexed() != needIndex) {
            elog("Property with name %s already exist but its type/valueType/indexFlag is inconsistent.", name);
            property = nullptr; 
        }
    }
    return property;
 }


int BeanWorld::undefineProperty(const char* name)
{
    if (name == nullptr) return 0;
    if (name[0] == 0) return 0;

    if (0 != m_db->undefineProperty_(name)) {
        elog("Failed to undefine property %s in database.", name);
        return -1;
    }
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return 0;
    Property* property = iter->second;
    delete property;
    m_propertyMap_.erase(iter);
    return 0;
}


// const Property* BeanWorld::getProperty(const char* name) const
// {
//     return const_cast<Property*>(((const BeanWorld*)this)->getProperty(name));
// }


Property* BeanWorld::getProperty(const char* name)
{
    if (name == nullptr || name[0] == 0) return nullptr;
    auto iter = m_propertyMap_.find(std::string(name));
    if (iter == m_propertyMap_.end()) {
        return nullptr;
    } else {
        return iter->second;
    }
}

const std::unordered_map<std::string, Property*>& BeanWorld::getProperties() 
{
    return m_propertyMap_;
};


int BeanWorld::reloadProperties()
{
    int err = 0;
    if (m_properties_loaded_)
        clear();
    err = m_db->loadProperties_(m_propertyMap_);
    if (err) {
        elog("%s", "Failed to load properties from database");
        clear();
        err = -1;
    } else {
        for (auto& item : m_propertyMap_) {
            //set world to this
            item.second->m_world_ = this;
        }
        m_properties_loaded_ = true;
    }
    return err;
}


int BeanWorld::unloadBean(oidType id)
{
    //todo
    return -1;
}


int BeanWorld::loadAll()
{
    //todo:
    return -1;
}


int BeanWorld::saveAll()
{
    //todo:
    return -1;
}


}
}
}
