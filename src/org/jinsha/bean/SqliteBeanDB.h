#pragma once

#include "./AbstractBeanDB.h"

#include <sqlite3.h>
#include <unordered_map>

namespace org {
namespace jinsha {
namespace bean {

class SqliteBeanDB : public AbstractBeanDB
{
public:
    // enum status_t {
    //     INIT = 0,
    //     CREATED,
    //     OPENED
    // };

    static const char* DB_PATH;
    static const char* PTABLE_NAME;
    static const char* OTABLE_NAME;

    SqliteBeanDB(const char* dir);
    ~SqliteBeanDB() override;

    virtual int connect_() override;
    virtual int disconnect_() override;

private:
    virtual int reInit_() override;

    virtual int defineProperty_(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) override;
    virtual int undefineProperty_(const char* name) override;

    virtual int loadProperties_(std::unordered_map<std::string, Property*>& properties) const override;

    virtual int createBean_(oidType &id);
    // virtual std::list<std::string> getBeanProperties_(oidType id) const override;

    // virtual Bean* getBean(oidType id) override;
    virtual int loadBeanBase_(oidType beanId, Json::Value& value, Json::Value& nativeData) override;
    virtual int saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value& nativeData) override;
    virtual int deleteBean_(Bean* bean) override;
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

    virtual int beginTransaction_() override;
    virtual int commitTransaction_() override;
    virtual int rollbackTransaction_() override;

private:
    int internalInit();
    const char* getDir()  {return m_dir;};
    int checkDB();
    int openDB();
    int closeDB();
    int getIdByPropertyIndex(const Property* property, oidType sid, Json::ArrayIndex index, sqlite3_int64& id) const;
    static bool determineDelayLoad(Property::Type type, Property::ValueType valueType);


private:
    const char* m_dir;
    std::string m_dbFullPath;
    sqlite3* m_sqlite3Db_;
    // int m_status = INIT;
    bool m_initialized;

};

}
}
}