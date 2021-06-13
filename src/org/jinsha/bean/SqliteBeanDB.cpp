
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"

#include <unistd.h>
#include <sqlite3.h>

 static int callback_select_ptable(void *data, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

namespace org {
namespace jinsha {
namespace bean {

const char* SqliteBeanDB::DB_PATH  =  "beandb.db";
const char* SqliteBeanDB::PTABLE_NAME = "ptable";
const char* SqliteBeanDB::OTABLE_NAME = "otable";

static const char* CREATE_PTABLE =  
 "CREATE TABLE PTABLE ( \
ID INT PRIMARY KEY NOT NULL, \
NAME VARCHAR(64) NOT NULL \
); ";

static const char* CREATE_OTABLE =  
 "CREATE TABLE OTABLE \
ID INT PRIMARY KEY NOT NULL, \
PROPERTY INT NOT NULL, \
VALUE INT NOT NULL \
); ";


SqliteBeanDB::SqliteBeanDB( const char* dir) : 
    AbstractBeanDB()
    , m_dir(dir)
    ,m_initialized(false)
    ,m_db(nullptr)
{
    if (dir == nullptr ||  dir[0] == 0) return;
    m_dbFullPath.append(m_dir).append("/").append(DB_PATH);
    if (checkDB() == -3) 
        m_initialized = init() == 0 ? true : false;
}


SqliteBeanDB::~SqliteBeanDB()
{
    close();
}


int SqliteBeanDB::open()
{
    if (!m_initialized) return -1;
    return openDB();
}


int SqliteBeanDB::close()
{
    if (!m_initialized) return -1;
    return closeDB();
}


int SqliteBeanDB::checkDB()
{
    if (m_dir == nullptr || m_dir[0] == 0) return -1;
    if (access(m_dir, 0) != 0) return -1; //dir does not exist
    if (access(m_dir, 7)) return -2; //not enough permission on this dir

    if (access(m_dbFullPath.c_str(), 0) != 0) return -3; //db file does not exist
    if (access(m_dbFullPath.c_str(), 6)) return -4; //not enough permission on the db file

    //todo: check database schema, data consistency etc. in future
    return 0;
}


int SqliteBeanDB::init()
{
    int errCode = 0;

    if (access(m_dbFullPath.c_str(), 0) == 0) return 0; //db file exists

    if (openDB()  !=  0) {
        elog("Failed to create database %s.\n", m_dbFullPath.c_str());
        errCode =  -2;
    } else {
        char *zErrMsg = 0;
        int  rc;
        const char *sql;

        /* Create SQL statement */
        sql = CREATE_PTABLE;

        /* Execute SQL statement */
        rc = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            elog("SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            errCode = -3;
        }else{
            ilog("%s", "PTABLE created successfully\n");
        }
        closeDB();
    }

    return errCode;
}

int SqliteBeanDB::loadAll()
{
    return 0;
}


int SqliteBeanDB::saveAll()
{
    return 0;
}


Bean* SqliteBeanDB::loadBean(oidType id)
{
    return 0;

}


int SqliteBeanDB::saveBean(Bean* bean)
{
    return 0;
}


int SqliteBeanDB::removeBean(Bean* bean)
{
    return 0;
}

int SqliteBeanDB::loadProperties()
{
    if (openDB() != 0) return -1;

    closeDB();
    return 0;
}

int SqliteBeanDB::saveProperty(Property* property)
{
    return 0;
}


int SqliteBeanDB::removeProperty(Property* property)
{
    return 0;
}


int SqliteBeanDB::openDB()
{
    int errCode = 0;
    if (m_db == nullptr)  {
        errCode = sqlite3_open(m_dbFullPath.c_str(), &m_db);
        if( errCode )  {
            elog("Failed to open database: %s\n", sqlite3_errmsg(m_db));
            m_db = nullptr;
        }
    }
    return errCode;
}

int SqliteBeanDB::closeDB()
{
    int errCode = 0;
    if (m_db != nullptr) {
        errCode = sqlite3_close(m_db);
        if( errCode )  {
            elog("Failed to close database: %s\n", sqlite3_errmsg(m_db));
        } else {
            m_db = nullptr;
        }
    }
    return errCode;
}

}
}
}