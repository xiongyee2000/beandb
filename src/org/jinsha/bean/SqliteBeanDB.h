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

    int connect() override;
    int disconnect() override;
    bool connected() const {return m_connected;};
    int clear() override;

    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineRelation(const char* name, bool needIndex = false) override;
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) override;
    virtual int undefineProperty(const char* name) override;
    virtual const Property* getProperty(const char* name) const override;
    virtual Property* getProperty(const char* name) override;
    int loadProperties() override;

    virtual Bean* createBean();
    virtual Bean* getBean(oidType id);
    virtual std::list<std::string> getBeanProperties(oidType id) const override;

    int loadAll() override;
    int saveAll() override;

    int updateBean(Bean* bean) override;
    int updateBean(Bean* bean, Property* property);
    int deleteBean(Bean* bean) override;
    Json::Value getBeanProperty(const Bean* bean, const Property* property) const override;
    virtual int insertBeanProperty(const Bean* bean, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty(const Bean* bean, 
        const Property* property, 
        const Json::Value& value) override;
    virtual int updateBeanProperty(const Bean* bean, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;
    virtual int deleteBeanProperty(const Bean* bean, 
        const Property* property) override;
    virtual int deleteBeanProperty(const Bean* bean, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;

    virtual int beginTransaction() override;
    virtual int commitTransaction() override;
    virtual int rollbackTransaction() override;

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
    bool m_inTransaction;

};

}
}
}