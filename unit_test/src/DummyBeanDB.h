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

    virtual int doBeginTransaction() override;
    virtual int doCommitTransaction() override;
    virtual int doRollbackTransaction() override;

    virtual int defineProperty(const char* name, 
        Property::Type type,  
        Property::ValueType valueType, 
        bool needIndex = false) override;
    virtual int undefineProperty(const char* name) override;
    virtual int loadProperties(std::unordered_map<std::string, Property*>& properties) const override;

    virtual Bean* createBean();
    virtual int loadBeanBase(oidType beanId, Json::Value& value, Json::Value& unmanagedValue) override;
    virtual int saveBeanBase(oidType beanId, const Json::Value& managedValue, const Json::Value& unmanagedValue) override;
    virtual int deleteBean(Bean* bean) override;

    virtual std::list<std::string> getBeanProperties(oidType id) const override;
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
    
    int loadAll() override;
    int saveAll() override;

    virtual int clear() override;

private:
    int internalInit();
    pidType m_maxPid = 0;
    std::unordered_map<std::string, Property*> m_properties;

private:
    bool m_connected;

};

}
}
}