#pragma once

#include "org/jinsha/beandb/AbstractBeanDB.h"

namespace org {
namespace jinsha {
namespace beandb {

class DummyBeanDB : public AbstractBeanDB
{
public:
    DummyBeanDB();
    ~DummyBeanDB() override;

    virtual int doConnect() override;
    virtual int doDisconnect() override;

    virtual int defineProperty(const char* name, 
        Property::Type type,  
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) override;
    virtual int undefineProperty(Property* property) override;

    virtual int loadProperties(std::unordered_map<std::string, Property*>& properties) const override;

    virtual int createBean(oidType& id) override;
    virtual int deleteBean(oidType id) override;

    virtual int loadBeanBase(oidType beanId, Json::Value& value, Json::Value* nativeData = nullptr) override;
    virtual int saveBeanBase(oidType beanId, const Json::Value& data, const Json::Value* nativeData = nullptr) override;

    // virtual std::list<std::string> getBeanProperties_(oidType id) const override;
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

    virtual int updateBeanNativeData(oidType beanId, 
        const Json::Value& value) override;
    virtual int deleteBeanNativeData(oidType beanId) override;
    virtual int loadBeanNativeData(oidType beanId, Json::Value& value) override;

    virtual int clear() override;

    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;
    virtual BeanIdPage* findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;
    virtual BeanIdPage* getAllBeans(unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    
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