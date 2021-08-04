#pragma once

#include "./AbstractBeanDB.h"

#include <sqlite3.h>

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

    virtual int connect() override;
    virtual int disconnect() override;
    virtual bool connected() const override {return m_connected;};
    virtual int clear() override;

    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineRelation(const char* name, bool needIndex = false) override;
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) override;
    virtual int undefineProperty(const char* name) override;
    virtual const Property* getProperty(const char* name) const override;
    virtual Property* getProperty(const char* name) override;
    int loadProperties() override;

    virtual Bean* createBean();
    virtual std::list<std::string> getBeanProperties(oidType id) const override;

    int loadAll() override;
    int saveAll() override;

    // virtual Bean* getBean(oidType id) override;
    virtual int loadBean(Bean* bean, Json::Value& value, Json::Value& unmanagedValue) override;
    virtual int saveBeanBase(const Bean* bean) override;
    virtual int deleteBean(Bean* bean) override;
    virtual int loadBeanProperty(const Bean* bean, const Property* property, Json::Value& value) override;
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
    
    virtual int loadUnmanagedValues(const Bean* bean, Json::Value& value) override;

    virtual int doBeginTransaction() override;
    virtual int doCommitTransaction() override;
    virtual int doRollbackTransaction() override;

private:
    int internalInit();
    Property* definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex = false);

private:
    const char* getDir()  {return m_dir;};
    int reInit(); //todo: tmp solution
    int checkDB();
    int openDB();
    int closeDB();
    int getIdByPropertyIndex(const char* pname, Json::ArrayIndex index, sqlite3_int64& id);


private:
    const char* m_dir;
    std::string m_dbFullPath;
    sqlite3* m_db;
    // int m_status = INIT;
    bool m_initialized;
    bool m_connected;

};

}
}
}