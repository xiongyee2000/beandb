#pragma once

#include "org/jinsha/bean/AbstractBeanDB.h"

namespace org {
namespace jinsha {
namespace bean {

class DummyBeanDB : public AbstractBeanDB
{
public:
    DummyBeanDB();
    ~DummyBeanDB() override;

    virtual int connect() override;
    virtual int disconnect() override;
    virtual bool connected() const override {return m_connected;};

    virtual int clear() override;

    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineRelation(const char* name, bool needIndex = false) override;
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) override;
    virtual int undefineProperty(const char* name) override;
    virtual int undefineRelation(const char* name) override;

    virtual int loadProperties(std::vector<std::string>& names, 
        std::vector<Property::Type>& types, 
        std::vector<Property::ValueType>& valueTypes,
        std::vector<bool>& indices) const override;

    virtual Bean* createBean();
    virtual std::list<std::string> getBeanProperties(oidType id) const override;

    int loadAll() override;
    int saveAll() override;

    virtual int loadBeanBase(oidType beanId, Json::Value& value, Json::Value& unmanagedValue) override;
    virtual int saveBeanBase(oidType beanId, const Json::Value& managedValue, const Json::Value& unmanagedValue) override;
    virtual int deleteBean(Bean* bean) override;
    virtual int loadBeanProperty(oidType beanId, const Property* property, Json::Value& value) override;
    virtual int insertBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;
    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property) override;
    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;

    virtual int insertBeanUnmanagedValue(oidType beanId, 
        const Json::Value& value) override;

    virtual int updateUnmanagedValue(oidType beanId, 
        const Json::Value& value) override;

    virtual int deleteBeanUnmanagedValue(oidType beanId, 
        const Json::Value& value) override;
    
    virtual int loadUnmanagedValue(oidType beanId, Json::Value& value) override;

    virtual int doBeginTransaction() override;
    virtual int doCommitTransaction() override;
    virtual int doRollbackTransaction() override;

private:
    int internalInit();
    Property* definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex = false);

private:
    bool m_connected;

};

}
}
}