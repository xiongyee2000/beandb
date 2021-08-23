
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
    ,m_connected(false)
{
}


DummyBeanDB::~DummyBeanDB()
{
    disconnect();
}


int DummyBeanDB::connect()
{
    m_connected = true;
    return 0;
}


int DummyBeanDB::disconnect()
{
    m_connected = false;
    return 0;
}

int DummyBeanDB::clear()
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


Bean* DummyBeanDB::createBean()
{
    BeanWorld *world = getWorld();
    return world->createBean(0);
}


int DummyBeanDB::loadBeanBase(oidType beanId, Json::Value& value, Json::Value& unmanagedValue) 
{
    return 0;
}


 std::list<std::string> DummyBeanDB::getBeanProperties(oidType id) const
{
    std::list<std::string> properties;
    return properties;
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


int DummyBeanDB::deleteBean(Bean* bean)
{
     return 0;
}


int DummyBeanDB::loadProperties(std::unordered_map<std::string, Property*>& properties) const
{
    return 0;
}


int DummyBeanDB::undefineProperty(const char* name)
{
    auto iter = m_properties.find(name);
    if (iter != m_properties.end()) {
        //todo: delete iter->second
        m_properties.erase(iter);
    }
    return 0;
}


pidType DummyBeanDB::defineProperty(const char* name, Property::Type type, 
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


int DummyBeanDB::doBeginTransaction() 
{
    return 0;
}


int DummyBeanDB::doCommitTransaction() 
{
    return 0;
}


int DummyBeanDB::doRollbackTransaction() 
{
    return 0;
}


int DummyBeanDB::saveBeanBase(oidType beanId, const Json::Value& managedValue, const Json::Value& unmanagedValue)
{
     return 0;
}

int DummyBeanDB::loadUnmanagedValue(oidType beanId, Json::Value& value)
{
    return 0;
}

int DummyBeanDB::insertBeanUnmanagedValue(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}

int DummyBeanDB::updateUnmanagedValue(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}

int DummyBeanDB::deleteBeanUnmanagedValue(oidType beanId, 
    const Json::Value& value)
{
    return 0;
}


}
}
}