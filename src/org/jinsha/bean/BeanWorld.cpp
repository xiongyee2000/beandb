
#include <assert.h>
#include <cstring>
#include "./BeanWorld.h"
#include "./internal_common.hxx"

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


void BeanWorld::clear()
{
    m_propertyMap_.clear();

    for (auto item : m_properties_)
    {
        delete item;
    }
    m_properties_.clear();

    for (auto item : m_beans_) 
    {
        delete item.second;
    }
    m_beans_.clear();
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


int BeanWorld::getPropertyId(const char* name) const
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;
    auto iter = m_propertyMap_.find(name);
    if (iter == m_propertyMap_.end()) return -1; //no such property
    return (int)(iter->second);
}


const Property* BeanWorld::getProperty(pidType pid) const
{
    if (pid < 0 || pid >= m_properties_.size())
        return nullptr;
    else
        return m_properties_[pid];
}


const Property* BeanWorld::getProperty(const char* name) const
{
    return getProperty(getPropertyId(name));
}


void BeanWorld::findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_eq, propertyName, value, beans);
}


void BeanWorld::findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_le, propertyName, value, beans);
}


void BeanWorld::findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_ge, propertyName, value, beans);
}


void BeanWorld::findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_lt, propertyName, value, beans);
}


void BeanWorld::findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    findCommon(op_gt, propertyName, value, beans);
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

    pidType  pid = getPropertyId(propertyName);
        //todo
        // m_propertyIndexTypes_[pid] = Json::objectValue;
}

template<typename T>
pidType BeanWorld::doSetProperty( Bean* bean, const char* name, T value)
{
    if (name == nullptr) return -1;
    if (name[0] == 0) return -1;

    pidType  pid = getPropertyId(name);
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
            property->m_refCount_++;
        }
    }
    
    //set value for json object
    auto& v = (*bean)[name];
    v = value;
    property->addIndex(bean, value);

    return pid;
}


pidType BeanWorld::setProperty(Bean* bean, const char* name, Json::Int value)
{
    return doSetProperty<Json::Int>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt value)
{
    return doSetProperty<Json::UInt>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, Json::Int64 value)
{
    return doSetProperty<Json::Int64>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, Json::UInt64 value)
{
    return doSetProperty<Json::UInt64>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, bool value)
{
    return doSetProperty<bool>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, double value)
{
    return doSetProperty<double>(bean, name, value);
}

pidType BeanWorld::setProperty(Bean* bean, const char* name, const char* value)
{
    if (value == nullptr) return -1;
    return doSetProperty<const char*>(bean, name, value);
}


Json::Value BeanWorld::removeProperty(Bean* bean, const char* name)
{
    if (name == nullptr) return Json::Value::null;
    if (name[0] == 0) return Json::Value::null;
    if (!bean->isMember(name)) return Json::Value::null;

    pidType pid = getPropertyId(name);
    if (pid != -1) 
    {
        auto& property = m_properties_[pid];
        property->removeIndex(bean, (*bean)[name]);
        property->m_refCount_--;
        if (property->m_refCount_ == 0)
        {
            removeProperty(pid);
        }
    }
    
    //remove member of json object
    return bean->m_jsonValue_.removeMember(name);
}

pidType BeanWorld::addProperty(const char* name)
{
    int pid = getPropertyId(name);
    if (pid == -1)
    {//no such property
        Property* property = new Property(name);
        property->m_refCount_ = 1;
        m_properties_.push_back(property);
        m_propertyMap_[name] = m_properties_.size() - 1;
        pid = m_properties_.size() - 1;
    }
    return pid;
}


void BeanWorld::removeProperty(pidType pid)
{
    Property* property = m_properties_[pid];
    auto iter = m_propertyMap_.find(property->getName());
    if (iter != m_propertyMap_.end()) m_propertyMap_.erase(iter);
    delete property;
    m_properties_[pid] = nullptr;
}


void BeanWorld::trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
    if (value.isNull() || value.isArray() || value.isObject()) return;
    beans.clear();
    Json::Value v;
    Bean* bean = nullptr;
    for (auto& item : m_beans_)
    {
        bean = item.second;
        v = bean->get(propertyName, Json::Value::null);
        if (v.isNull()) continue; //not found or null
        switch (opType) {
            case op_eq:
                if (v == value) beans.push_back(bean);
                break;
            case op_le:
                if (v <= value) beans.push_back(bean);
                break;
            case op_ge:
                if (v >= value) beans.push_back(bean);
                break;
            case op_lt:
                if (v < value) beans.push_back(bean);
                break;
            case op_gt:
                if (v > value) beans.push_back(bean);
                break;
            default:
                break;
        }
    }
}


void BeanWorld::findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
   const Property* property = getProperty(propertyName);
    if (property != nullptr)
    { //indexed by property, use index to improve performance
        // switch (type)
        property->findCommon(opType, value, beans);
    }
    else
    { //no index, do trivial find
        trivialFind(opType, propertyName, value, beans);
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
