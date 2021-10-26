
#include "./DummyBeanDB.h"
#include "org/jinsha/bean/internal_common.hxx"
#include "org/jinsha/bean/Bean.h"

#include <unistd.h>
#include <string.h>
#include "jsoncpp/json/value.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"

namespace org {
namespace jinsha {
namespace bean {

DummyBeanDB::DummyBeanDB() 
    : AbstractBeanDB()
{
}


DummyBeanDB::~DummyBeanDB()
{
    disconnect();
}


int DummyBeanDB::doConnect()
{
    return 0;
}


int DummyBeanDB::doDisconnect()
{
    return 0;
}

int DummyBeanDB::clear()
{
    return 0;
}


int DummyBeanDB::createBean(oidType& id) 
{
    id = m_maxOid++;
    m_dataMap_[id] = Json::Value(Json::objectValue);
    m_nativeDataMap_[id] = Json::Value(Json::objectValue);
    return 0;
}


int DummyBeanDB::deleteBean(oidType id) 
{
    m_dataMap_.erase(id);
    m_nativeDataMap_.erase(id);
    return 0;
}


int DummyBeanDB::saveBeanBase(oidType beanId, const Json::Value& data, const Json::Value* nativeData)
{

    m_dataMap_[beanId] = data;
    if (nativeData != nullptr)
        m_nativeDataMap_[beanId] = *nativeData;
    return 0;
}


int DummyBeanDB::loadBeanBase(oidType beanId, Json::Value& value, Json::Value* nativeData) 
{
    int err = 0;
    auto iter = m_dataMap_.find(beanId);
    if (iter == m_dataMap_.end()) return 1;
    value = iter->second;
    if (nativeData != nullptr) *nativeData = m_nativeDataMap_[beanId];
    return 0;
}


int  DummyBeanDB::loadBeanProperty(oidType beanId, const Property* property, Json::Value& value)
{
    return 0;

}


int DummyBeanDB::insertBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    return 0;
}


int DummyBeanDB::updateBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    return updateBeanProperty(beanId, property, (Json::ArrayIndex)-1, value);
}


int DummyBeanDB::updateBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) 
{
    return 0;
}

int DummyBeanDB::deleteBeanProperty(oidType beanId, 
    const Property* property) 
{
    return deleteBeanProperty(beanId, property, (Json::Value::ArrayIndex)-1);
}

int DummyBeanDB::deleteBeanProperty(oidType beanId, 
    const Property* property, 
    Json::Value::ArrayIndex index) 
{
    return 0;
}


int DummyBeanDB::loadProperties(std::unordered_map<std::string, Property*>& properties) const
{
    properties = m_properties_;
    return 0;
}


int DummyBeanDB::undefineProperty(Property* property)
{
    if (property == nullptr) return 0;
    const auto& name = property->getName();
    auto iter = m_properties_.find(name);
    if (iter != m_properties_.end()) {
        //todo: delete iter->second
        m_properties_.erase(iter);
    }
    return 0;
}


pidType DummyBeanDB::defineProperty(const char* name, Property::Type type, 
    Property::ValueType valueType, pidType& pid, bool& delayLoad)
{
    auto iter = m_properties_.find(name);
    if (iter == m_properties_.end()) {
        pid = m_maxPid++;
        delayLoad = (valueType == Property::StringType);
        Property* property = newProperty(name,  
                                                                                pid, 
                                                                                (Property::Type)type, 
                                                                                (Property::ValueType)valueType, 
                                                                                delayLoad);
        m_properties_[name] = property;
    } else {
        pid = iter->second->getId();
    }
    return 0;
}


int DummyBeanDB::loadBeanNativeData(oidType beanId, Json::Value& value)
{
    if (m_nativeDataMap_.find(beanId) == m_nativeDataMap_.end()) {
        return 1;
    }
    value = m_nativeDataMap_[beanId];
    return 0;
}


int DummyBeanDB::updateBeanNativeData(oidType beanId, 
    const Json::Value& value)
{
    int err = 0;
    if (m_nativeDataMap_.find(beanId) == m_nativeDataMap_.end()) {
        err = 1;
    } else {
        m_nativeDataMap_[beanId] = value;
    }
    return err;
}

int DummyBeanDB::deleteBeanNativeData(oidType beanId)
{
    int err = 0;
    auto iter = m_nativeDataMap_.find(beanId) ;
    if (iter != m_nativeDataMap_.end()) {
        m_nativeDataMap_.erase(iter);
    }
    return err;
}


BeanIdPage* DummyBeanDB::findSubjects(const Property* property, unsigned int pageSize) const
{
    return nullptr;
}


BeanIdPage* DummyBeanDB::findObjects(const Property* property, unsigned int pageSize) const
{
    return nullptr;
}


BeanIdPage* DummyBeanDB::findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize) const
{
    return nullptr;
}


BeanIdPage* DummyBeanDB::getAllBeans(unsigned int pageSize) const
{
    return nullptr;
}


}
}
}