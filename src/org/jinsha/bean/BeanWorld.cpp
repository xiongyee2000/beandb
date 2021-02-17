
#include <assert.h>
#include <cstring>
#include <locale.h>

#include "./BeanWorld.h"
#include "./internal_common.hxx"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {


BeanWorld::BeanWorld()
{
    setlocale(LC_ALL, "");
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
    if (pid < 0 || (size_t)pid >= m_properties_.size())
        return nullptr;
    else
        return m_properties_[pid];
}


const Property* BeanWorld::getProperty(const char* name) const
{
    return getProperty(getPropertyId(name));
}


pidType BeanWorld::createIndex(const char* propertyName)
{
    if (propertyName == nullptr) return -1;
    if (propertyName[0] == 0) return -1;
    pidType pid = getPropertyId(propertyName);
    if (pid == -1)
    {
        pid = addProperty(propertyName);
        Property* property = (Property*)getProperty(pid);
        property->m_indexed_ = true;
        updateIndex(property);
    }
    return pid;
}


pidType BeanWorld::removeIndex(pidType pid)
{
    if (pid < 0 || pid >= m_properties_.size()) return -1;
    Property* property = m_properties_[pid];
    if (property != nullptr) 
    {
        property->clear();
        property->m_indexed_ = false;
        return pid;
    }
    else
    {
        return -2;
    }
}


pidType BeanWorld::updateIndex(Property* property)
{
    Bean* bean = nullptr;
    const string& pname = property->getName();
    Json::Value value;
    property->clear();
    for (auto& iter : m_beans_)
    {
        bean = iter.second;
        value = bean->getProperty(pname.c_str());
        if (!value.isNull())
        {
            property->addIndex(bean, value);
            property->m_refCount_++;
        }
    }
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
    // if (propertyName == nullptr) return -1;
    // if (propertyName[0] == 0) return -1;

    // pidType  pid = addProperty(propertyName);
    // (*from)[propertyName] = to->getId();
}


int BeanWorld::removeRelation(Bean* from, Bean* to, const char* propertyName)
{
    // if (propertyName == nullptr) return -1;
    // if (propertyName[0] == 0) return -1;

    // from->removeProperty(propertyName);

    // pidType  pid = getPropertyId(propertyName);
        //todo
        // m_propertyIndexTypes_[pid] = Json::objectValue;
}


pidType BeanWorld::setProperty( Bean* bean, const char* name, const Json::Value& value)
{
    pidType  pid = getPropertyId(name);
    if (pid == -1)
    {
        pid = addProperty(name);
    }
    Property* property = m_properties_[pid]; //property won't be null here
    doSetProperty(bean, property, value);
    return pid;
}


void BeanWorld::setProperty( Bean* bean, pidType pid, const Json::Value& value)
{
    Property* property = m_properties_[pid];
    if (property != nullptr)
        doSetProperty(bean, property, value);
}


void BeanWorld::doSetProperty( Bean* bean, Property* property, const Json::Value& value)
{
    const string& pname = property->getName();
    Json::Value&& oldValue = bean->getProperty(pname.c_str());
    if (oldValue.isNull())
    { //no old value, new property added to bean
        property->m_refCount_++;
    }
    else
    {
        if (property->m_indexed_)
        {
            //remove index for previous value
            if (!oldValue.isNull() && !oldValue.isObject() && !oldValue.isArray())
                property->removeIndex(bean, oldValue);
        }
    }

    //set value for json object
    Json::Value& v =bean->m_jsonValue_[pname];
    v  = value;
    if (property->m_indexed_)
        property->addIndex(bean, v);
    
}


Json::Value BeanWorld::removeProperty(Bean* bean,  Property* property)
{
    const char* pname = property->getName().c_str();
    Json::Value value;
    if (bean->isMember(pname))
    {
        if (property->m_indexed_)
           //remove index first
            property->removeIndex(bean, bean->getProperty(pname));
        //remove member of json object
        value = bean->m_jsonValue_.removeMember(pname);
        property->m_refCount_--;
        if (property->m_refCount_ == 0)
        { //if this property is not used by any bean, remove it
            removeProperty(property->m_pid_);
        }
    }

    return value;
}


pidType BeanWorld::addProperty(const char* name)
{
    int pid = getPropertyId(name);
    if (pid == -1)
    {//no such property
        Property* property = new Property(name);
        m_properties_.push_back(property);
        m_propertyMap_[name] = m_properties_.size() - 1;
        pid = m_properties_.size() - 1;
        property->m_pid_ = pid;
    }
    return pid;
}


void BeanWorld::removeProperty(pidType pid)
{
    Property* property = m_properties_[pid];
    if (property == nullptr) return;
    auto iter = m_propertyMap_.find(property->getName());
    if (iter != m_propertyMap_.end()) m_propertyMap_.erase(iter);
    delete property;
    m_properties_[pid] = nullptr;
}


void BeanWorld::findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans)
{
   const Property* property = getProperty(propertyName);
   if (property == nullptr) return;
    if (property->m_indexed_)
    { //indexed by property, use index to improve performance
        // switch (type)
        property->findCommon(opType, value, beans);
    }
    else
    { //no index, do trivial find
        trivialFind(opType, propertyName, value, beans);
    }
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
        v = bean->getProperty(propertyName);
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


oidType BeanWorld::generateBeanId()
{
    //todo: currently just return m_maxBeanId_++ assuming it is enough.
    return m_maxBeanId_++;
};


}
}
}
