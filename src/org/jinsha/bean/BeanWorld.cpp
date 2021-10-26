
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
    doUnloadAll(false);
};


void BeanWorld::unloadAll()
{
    //todo: handle err here?
    doUnloadAll(true);
}


int BeanWorld::doUnloadAll(bool reloadProperties)
{
    int err = 0;

    unloadAllBeans();
    unloadProperties();

    if (reloadProperties)  {
        err = loadProperties();
    }
    m_properties_loaded_ = false;

   return err;
}


Bean* BeanWorld::newBean()
{
    int err = 0;
    oidType id = 0;
    Bean* bean = nullptr;
    err  = m_db_->createBean(id);
    if (err) {
        elog("Failed to create bean in database. (err=%d) \n", err);
    } else {
        bean = new Bean(id, this);
        assert(bean);
        m_beans_[id] = bean;
    } 
    return bean;
};


void BeanWorld::unloadBean(Bean* bean)
{
    if (bean == nullptr) return;
    auto iter = m_beans_.find(bean->getId());
    if (iter == m_beans_.end()) return;
    m_beans_.erase(iter);
    bean->unload();
    delete bean;
}


void BeanWorld::unloadAllBeans()
{
    for (auto& item : m_beans_) {
        item.second->unload();
        delete item.second;
    }
    m_beans_.clear();
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
    Json::Value data;
    Json::Value nativeData;
    Bean* bean = nullptr;
    auto iter = m_beans_.find(id);
    if (iter == m_beans_.end())
    { //bean not found in this world
        if (!loadFromDB) return nullptr;
        //try to load it from db
        if (m_db_ == nullptr) return nullptr;
        if (!m_db_->connected()) return nullptr;
        bean = new Bean(id, this);
        err = m_db_->loadBeanBase(id, data);
        if (err) {
            delete bean;
            bean = nullptr;
        } else {
            //todo: 
            //check data validity, e.g.: property name consistency, 
            //delay load attribute etc...
            
            err = bean->load(data);
            if (err) {
                delete bean;
                bean = nullptr;
            } else {
                m_beans_[id] = bean;
            }
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
    oidType subjectId = 0;
    Bean* subject = nullptr;
    Json::Value value;
    Property* property = nullptr;
    err = m_db_->deleteBean(bean->getId());
    if (!err) {
        auto iter = bean->m_subjectMap_.begin();
        while ( iter != bean->m_subjectMap_.end()) {
            subjectId = iter->first;
            property = iter->second;
            if (subjectId != bean->getId()) {
                subject = getBean(subjectId, false);
                if (subject == nullptr)  continue;
                if (property->isArray()) {
                    for (int i = 0; i < subject->size(property); i++) {
                        if (subject->getObjectId(property, i) == bean->getId())
                            subject->doRemoveProperty(property, i, false);
                    }
                } else {
                    subject->doRemoveProperty(property, false);
                }
            }
            iter = bean->m_subjectMap_.begin();
        }
        unloadBean(bean);
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
        err = m_db_ ->defineProperty(name, type, valueType, pid, delayLoad);
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

    if (0 != m_db_->undefineProperty(property)) {
        elog("Failed to undefine property %s in database.", name);
        return -1;
    }

    Bean* bean = nullptr;
    //remove property from beans that have this property
    auto iter2 = property->m_subjectMap_.begin();
    while (property->m_subjectMap_.end() != iter2)
    {
        bean = getBean(iter2->first, false);
        if (bean != nullptr) {
            bean->doRemoveProperty(property, false);
        }
        iter2 = property->m_subjectMap_.begin();
    }
    m_propertyMap_.erase(iter);
    delete property;
    return 0;
}


// const Property* BeanWorld::getProperty(const char* name) const
// {
//     return const_cast<Property*>(((const BeanWorld*)this)->getProperty(name));
// }


Property* BeanWorld::getProperty(const char* name) const
{
    if (name == nullptr || name[0] == 0) return nullptr;
    auto iter = m_propertyMap_.find(std::string(name));
    if (iter == m_propertyMap_.end()) {
        return nullptr;
    } else {
        return iter->second;
    }
}


Property* BeanWorld::getProperty(pidType id) const
{
    for (auto& item : m_propertyMap_) {
        if (item.second->getId() == id) {
            return item.second;
        }
    }
    return nullptr;
}


const std::unordered_map<std::string, Property*>& BeanWorld::getProperties()  const 
{
    return m_propertyMap_;
};


int BeanWorld::loadProperties()
{
    int err = 0;
    err = m_db_->loadProperties(m_propertyMap_);
    if (err) {
        elog("%s", "Failed to load properties from database");
        unloadProperties();
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


void BeanWorld::unloadProperties()
{
    for (auto& item : m_propertyMap_) 
        delete item.second;
    m_propertyMap_.clear();
}


int BeanWorld::loadAll()
{
    if (m_db_ == nullptr || !m_db_->connected()) return -1;
    
    int err = 0;
    oidType beanId = 0;

    doUnloadAll(false);
    err = loadProperties();
    if (err) return err;

    BeanIdPage* page = m_db_->getAllBeans();
    if (page != nullptr) {
        do {
            for (int i = 0; i < page->size(); i++) {
                beanId = page->at(i);
                getBean(beanId, true);
            }
            err = page->next();
        } while (!err);
    } else {
        err =  -1;
    }

    if (err == -1001) err = 0;
    return err;
}


int BeanWorld::saveAll()
{
    if (m_db_ == nullptr || !m_db_->connected()) return -1;
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


 BeanIdPage* BeanWorld::getAllBeans(unsigned int pageSize) const
 {
    if (m_db_ == nullptr || !m_db_->connected()) return nullptr;
    return m_db_->getAllBeans(pageSize);
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
    if (m_db_ == nullptr || !m_db_->connected()) return nullptr;
    return m_db_->findSubjects(property, pageSize);
}


BeanIdPage* BeanWorld::findObjects(const Property* property, unsigned int pageSize) const
{
    if (m_db_ == nullptr || !m_db_->connected()) return nullptr;
    return m_db_->findObjects(property, pageSize);
}


BeanIdPage* BeanWorld::findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    if (m_db_ == nullptr || !m_db_->connected()) return nullptr;
    return m_db_->findBeans(optype, property, value, pageSize);
}

}
}
}
