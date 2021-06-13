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

    int open();
    int close();
    int checkDB();

    int loadAll() override;
    int saveAll() override;

    Bean* loadBean(oidType id) override;
    int saveBean(Bean* bean) override;
    int removeBean(Bean* bean) override;

    int loadProperties() override;
    int saveProperty(Property* property) override;
    int removeProperty(Property* property) override;

public:
    const char* getDir()  {return m_dir;};

private:
    int init() override;


private:
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