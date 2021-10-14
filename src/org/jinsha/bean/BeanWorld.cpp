
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
    : m_db_(&db)
{
    setlocale(LC_ALL, "");
    m_db_->m_world_ = this;
}


BeanWorld::~BeanWorld()
{
    clear();

    for (auto& item : m_propertyMap_) 
    {
        delete item.second;
    }
   m_propertyMap_.clear();
   m_properties_loaded_ = false;
};


void BeanWorld::clear()
{
    for (auto& item : m_beans_) 
    {
        delete item.second;
        item.second = nullptr;
    }
    m_beans_.clear();
}


Bean* BeanWorld::createBean()
{
    int err = 0;
    oidType id = 0;
    Bean* bean = nullptr;
    err  = m_db_->createBean_(id);
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
            subject = getBean(iter2->first, false);
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


int BeanWorld::getCachedNumOfBeans()
{
    return m_beans_.size();
}

const unordered_map<oidType, Bean*>& BeanWorld::getCachedBeans()
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
    err = m_db_->deleteBean_(bean->getId());
    if (!err) {
        removeBean(bean->getId());
    }
    
    return err;
}


Property* BeanWorld::defineProperty(const char* name, Property::ValueType valueType)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType);
}


Property* BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType);
}


Property* BeanWorld::defineRelation(const char* name)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType);
}


Property* BeanWorld::defineArrayRelation(const char* name)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType);
}


Property* BeanWorld::definePropertyCommon_(const char* name, 
    Property::Type type, 
    Property::ValueType valueType)
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
        err = m_db_ ->defineProperty_(name, type, valueType, pid, delayLoad);
        if (err) {
            elog("Failed to define property %s in database.", name);
            return nullptr;
        } else {
            property = new Property(this, name, pid, type, valueType, delayLoad);
            m_propertyMap_[name] = property;
            property->m_id_ = pid;
        }
    }
    else
    {
        property = iter->second;
        if (property->getType() != type || 
            property->getValueType() != valueType) {
            elog("Property with name %s already exist but its type/valueType is inconsistent.", name);
            property = nullptr; 
        }
    }
    return property;
 }


int BeanWorld::undefineProperty(Property* property)
{
    if (property == nullptr) return 0;
    const auto& name = property->getName();
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return 0;

    if (0 != m_db_->undefineProperty_(property)) {
        elog("Failed to undefine property %s in database.", name);
        return -1;
    }

    m_propertyMap_.erase(iter);
    delete property;
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


Property* BeanWorld::getProperty(pidType id)
{
    for (auto& item : m_propertyMap_) {
        if (item.second->getId() == id) {
            return item.second;
        }
    }
    return nullptr;
}


const std::unordered_map<std::string, Property*>& BeanWorld::getProperties() 
{
    return m_propertyMap_;
};


// const std::unordered_map<std::string, Property*>& BeanWorld::getRelations() 
// {
//     return m_relationMap_;
// };


int BeanWorld::reloadProperties()
{
    int err = 0;
    if (m_properties_loaded_)
        clear();
    err = m_db_->loadProperties_(m_propertyMap_);
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


int BeanWorld::loadAll()
{
    //todo:
    return -1;
}


int BeanWorld::saveAll()
{
    if (!m_db_->connected()) return -1;
    int err = 0;
    for (auto& iter : m_beans_) {
        err = iter.second->save();
        if (err) {
            elog("Failed in %s at bean: id=%d \n", __func__, iter.second->getId());
            break;
        } 
    }
    return err;
}


BeanIdPage* BeanWorld::findEqual(const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return findBeans(op_eq, property, value, pageSize);
}


BeanIdPage* BeanWorld::findLessEqual(const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return findBeans(op_le, property, value, pageSize);
}


BeanIdPage* BeanWorld::findLessThan(const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return findBeans(op_lt, property, value, pageSize);
}


BeanIdPage* BeanWorld::findGreaterEqual(const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return findBeans(op_ge, property, value, pageSize);
}


BeanIdPage* BeanWorld::findGreaterThan(const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return findBeans(op_gt, property, value, pageSize);
}


BeanIdPage* BeanWorld::findSubjects(const Property* property, unsigned int pageSize) const
{
    return nullptr;
}


BeanIdPage* BeanWorld::findObjects(const Property* property, unsigned int pageSize) const
{
    return nullptr;
}


BeanIdPage* BeanWorld::findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    if (m_db_ != nullptr && m_db_->connected()) return nullptr;
    return m_db_->findBeans(optype, property, value, pageSize);
}

}
}
}
