
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


int DummyBeanDB::connect_()
{
    return 0;
}


int DummyBeanDB::disconnect_()
{
    return 0;
}

int DummyBeanDB::reInit_()
{
    return 0;
}


int DummyBeanDB::loadAll()
{
    return 0;
}


int DummyBeanDB::saveAll()
{
    return 0;
}

int DummyBeanDB::createBean_(oidType& id) 
{
    id = m_maxOid++;
    return 0;
}


int DummyBeanDB::deleteBean_(Bean* bean) 
{
    return 0;
}

int DummyBeanDB::loadBeanBase_(oidType beanId, Json::Value& value, Json::Value& nativeData) 
{
    return 0;
}


 std::list<std::string> DummyBeanDB::getBeanProperties_(oidType id) const
{
    std::list<std::string> properties;
    return properties;
}


int  DummyBeanDB::loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value)
{
    return 0;

}


int DummyBeanDB::insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    return 0;
}


int DummyBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    return updateBeanProperty_(beanId, property, (Json::ArrayIndex)-1, value);
}


int DummyBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) 
{
    return 0;
}

int DummyBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property) 
{
    return deleteBeanProperty_(beanId, property, (Json::Value::ArrayIndex)-1);
}

int DummyBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property, 
    Json::Value::ArrayIndex index) 
{
    return 0;
}


int DummyBeanDB::loadProperties_(std::unordered_map<std::string, Property*>& properties) const
{
    return 0;
}


int DummyBeanDB::undefineProperty_(const char* name)
{
    auto iter = m_properties.find(name);
    if (iter != m_properties.end()) {
        //todo: delete iter->second
        m_properties.erase(iter);
    }
    return 0;
}


pidType DummyBeanDB::defineProperty_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
    auto iter = m_properties.find(name);
    if (iter == m_properties.end()) {
        pidType id = m_maxPid++;
        Property* property = newProperty(name, id, type, valueType, needIndex);
        m_properties[name] = property;
        return id;
    }
    return iter->second->getId();
}


int DummyBeanDB::beginTransaction_() 
{
    return 0;
}


int DummyBeanDB::commitTransaction_() 
{
    return 0;
}


int DummyBeanDB::rollbackTransaction_() 
{
    return 0;
}


int DummyBeanDB::saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value& nativeData)
{
     return 0;
}

int DummyBeanDB::loadBeanNativeData_(oidType beanId, Json::Value& value)
{
    return 0;
}

int DummyBeanDB::insertBeanNativeData_(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}

int DummyBeanDB::updateBeanNativeData_(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}

int DummyBeanDB::deleteBeanNativeData_(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}


}
}
}