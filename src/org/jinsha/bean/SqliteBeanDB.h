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
    int clear() override;

    int loadAll() override;
    int saveAll() override;

    Bean* loadBean(oidType id) override;
    int saveBean(Bean* bean) override;
    int removeBean(Bean* bean) override;

    virtual Property* defineProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex = false) override;
    virtual Property* defineRelation(const char* name, bool needIndex = false) override;
    virtual Property* defineArrayRelation(const char* name, bool needIndex = false) override;
    virtual int undefineProperty(const char* name) override;
    virtual const Property* getProperty(const char* name) const override;
    virtual Property* getProperty(const char* name) override;

    int loadProperties() override;

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

private:
    const char* m_dir;
    std::string m_dbFullPath;
    sqlite3* m_db;
    // int m_status = INIT;
    bool m_initialized;
};

}
}
}