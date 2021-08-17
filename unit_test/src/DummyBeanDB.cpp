
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


// int DummyBeanDB::loadProperties(std::list<std::string> propertyNames) const
int DummyBeanDB::loadProperties() const
{
    return 0;
}


int DummyBeanDB::undefineProperty(const char* name)
{
    return 0;
}


int DummyBeanDB::undefineRelation(const char* name) 
{
    return undefineProperty(name);
}


Property* DummyBeanDB::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


Property* DummyBeanDB::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


Property* DummyBeanDB::defineRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


Property* DummyBeanDB::defineArrayRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


Property* DummyBeanDB::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
     return 0;
}


const Property* DummyBeanDB::getProperty(const char* name) const
{
    return (const Property*) ((DummyBeanDB*)this)->getProperty(name);
}


Property* DummyBeanDB::getProperty(const char* name)
{
     return 0;
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