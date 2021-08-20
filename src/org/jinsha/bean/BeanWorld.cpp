
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
    m_db->m_world = this;
    m_db->init();
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
}


Bean* BeanWorld::createBean(oidType id)
{
    Bean* bean = new Bean(this);
    assert(bean);
    if (id == 0)
        bean->m_id_ = generateBeanId();
    else 
        bean->m_id_ = id;
    m_beans_[bean->m_id_] = bean;
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
    return m_db->defineProperty(name, valueType, needIndex);
}


Property* BeanWorld::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return m_db->defineProperty(name, valueType, needIndex);
}


Property* BeanWorld::defineRelation(const char* name, bool needIndex)
{
    return m_db->defineRelation(name, needIndex);
}


Property* BeanWorld::defineArrayRelation(const char* name, bool needIndex)
{
    return m_db->defineArrayRelation(name, needIndex);
}


int BeanWorld::undefineProperty(const char* name)
{
    return m_db->undefineProperty(name);
}


const Property* BeanWorld::getProperty(const char* name) const
{
    return m_db->getProperty(name);
}


Property* BeanWorld::getProperty(const char* name)
{
    return const_cast<Property*>(((const BeanWorld*)this)->getProperty(name));
}

const std::unordered_map<std::string, Property*>& BeanWorld::getProperties() const 
{
    return m_db->getProperties();
};


int BeanWorld::loadAll()
{
    //todo
    return -1;
}


int BeanWorld::unloadAll()
{
    //todo
    return -1;
}


int BeanWorld::loadProperties()
{
    int err = 0;
    if (m_db != nullptr) {
        //todo: tmp solution
        m_db->getProperty("@#$%");
    } 
    return err;
}


int BeanWorld::unloadBean(oidType id)
{
    //todo
    return -1;
}


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
