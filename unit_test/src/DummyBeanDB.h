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

    virtual int connect_() override;
    virtual int disconnect_() override;

    virtual int beginTransaction_() override;
    virtual int commitTransaction_() override;
    virtual int rollbackTransaction_() override;

    virtual int defineProperty_(const char* name, 
        Property::Type type,  
        Property::ValueType valueType, 
        bool needIndex,
        pidType& pid,
        bool& delayLoad) override;
    virtual int undefineProperty_(const char* name) override;

    virtual int loadProperties_(std::unordered_map<std::string, Property*>& properties) const override;

    virtual int createBean_(oidType& id) override;
    virtual int deleteBean_(Bean* bean) override;

    virtual int loadBeanBase_(oidType beanId, Json::Value& value, Json::Value& nativeData) override;
    virtual int saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value& nativeData) override;

    // virtual std::list<std::string> getBeanProperties_(oidType id) const override;
    virtual int loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value) override;
    virtual int insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property) override;
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;

    virtual int updateBeanNativeData_(oidType beanId, 
        const Json::Value& value) override;
    virtual int deleteBeanNativeData_(oidType beanId) override;
    virtual int loadBeanNativeData_(oidType beanId, Json::Value& value) override;

    virtual int reInit_() override;

private:
    int internalInit();

private:
    std::unordered_map<oidType, Json::Value> m_dataMap_;
    std::unordered_map<oidType, Json::Value> m_nativeDataMap_;
    pidType m_maxPid = 1;
    oidType m_maxOid = 1;
    std::unordered_map<std::string, Property*> m_properties_;

};

}
}
}