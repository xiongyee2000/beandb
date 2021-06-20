
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"

#include <unistd.h>
#include <string.h>
#include <sqlite3.h>

namespace org {
namespace jinsha {
namespace bean {

const char* SqliteBeanDB::DB_PATH  =  "beandb.db";
const char* SqliteBeanDB::PTABLE_NAME = "ptable";
const char* SqliteBeanDB::OTABLE_NAME = "otable";

static const char* CREATE_PTABLE =  
 "CREATE TABLE PTABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
NAME VARCHAR(64) NOT NULL UNIQUE, \
PTYPE INT NOT NULL, \
VTYPE INT NOT NULL \
); ";

static const char* CREATE_OTABLE =  
 "CREATE TABLE OTABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
PROPERTY UNSIGNED BIG INT NOT NULL, \
VALUE UNSIGNED BIG INT NOT NULL \
); ";

static const char* CREATE_STABLE =  
 "CREATE TABLE STABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
VALUE TEXT NOT NULL \
); ";

SqliteBeanDB::SqliteBeanDB( const char* dir) : 
    AbstractBeanDB()
    , m_dir(dir)
    ,m_db(nullptr)
    ,m_initialized(false)
{
    if (dir == nullptr ||  dir[0] == 0) return;
    m_dbFullPath.append(m_dir).append("/").append(DB_PATH);
    if (checkDB() >= 0)
        m_initialized = internalInit() == 0 ? true : false;
}


SqliteBeanDB::~SqliteBeanDB()
{
    disconnect();
}


int SqliteBeanDB::connect()
{
    if (!m_initialized) return -1;
    if (m_db != nullptr) return 0;
    return openDB();
}


int SqliteBeanDB::disconnect()
{
    if (!m_initialized) return 0;
    return closeDB();
}

int SqliteBeanDB::clear()
{
    return reInit();
}


int SqliteBeanDB::checkDB()
{
    if (m_dir == nullptr || m_dir[0] == 0) return -1;
    if (access(m_dir, 0) != 0) return -1; //dir does not exist
    if (access(m_dir, 7)) return -2; //not enough permission on this dir

    if (access(m_dbFullPath.c_str(), 0) != 0) return 1; //db file does not exist
    if (access(m_dbFullPath.c_str(), 6)) return 2; //not enough permission on the db file

    //todo: check database schema, data consistency etc. in future
    return 0;
}


int SqliteBeanDB::internalInit()
{
    int errCode = 0;

    if (access(m_dbFullPath.c_str(), 0) == 0) return 0; //db file exists

    if (openDB()  !=  0) {
        elog("Failed to create database %s.\n", m_dbFullPath.c_str());
        return -1;
    } 

    char *zErrMsg = nullptr;
    const char *sql = nullptr;

    sql = CREATE_PTABLE;
    errCode = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( errCode != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return errCode;
    } 
    ilog("%s", "PTABLE created successfully\n");

    sql = CREATE_OTABLE;
    errCode = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( errCode != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return errCode;
    }
    ilog("%s", "OTABLE created successfully\n");
    
    sql = CREATE_STABLE;
    errCode = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( errCode != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return errCode;
    }
    ilog("%s", "STABLE created successfully\n");
    
    closeDB();
    return 0;
}


int SqliteBeanDB::reInit()
{
    if (m_dir == nullptr || m_dir[0] == 0) return -1;
    char buff[256] = {0};
    snprintf(buff, 255, "rm -rf %s/*", m_dir );
    //todo: check if command is executed successfully
    system(buff);
    int errCode = internalInit();
    m_initialized = errCode == 0 ? true : false;
    return errCode;
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
    if (m_db == nullptr) return 0;
    errCode = sqlite3_close(m_db);
    if( errCode )  {
        elog("Failed to close database: %s\n", sqlite3_errmsg(m_db));
    } else {
        m_db = nullptr;
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


//  static int loadPropertiesCallback(void *param, int columnCount, char **columnValue, char **columnName)
//  {
//      SqliteBeanDB *sqliteBeanDB = (SqliteBeanDB*)param;
//      BeanWorld *world = sqliteBeanDB->getWorld();
//      if (world == nullptr) return -1;
//      Property *property;
//     for (int i = 0; i < columnCount; i++) {
//         printf("%s = %s\n", columnName[i], columnValue[i]);
//     }

//      return 0;
//  }

int SqliteBeanDB::loadProperties()
{
    static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE FROM PTABLE;";
    BeanWorld *world = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int errCode = 0;
    int type = 0;
    int valueType = 0;
    const char *name = nullptr;
    // int64_t id = 0;

    if (m_db == nullptr) return -1;
    if ((world = getWorld()) == nullptr) return -2;

	errCode = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, &pzTail);
    if (errCode != SQLITE_OK) return errCode;

	while((errCode = sqlite3_step( pstmt )) == SQLITE_ROW) {
        //todo: set id for the property in future
        // nCol = 0;
        // id = sqlite3_column_int64(pstmt, nCol++);
        nCol = 1;
        name = (const char*)sqlite3_column_text(pstmt, nCol++);
        type = sqlite3_column_int(pstmt, nCol++);
        valueType = sqlite3_column_int(pstmt, nCol++);
        switch (type) {
            case Property::PrimaryType:
                world->defineProperty(name, (Property::ValueType)valueType);
                break;
            case Property::RelationType:
                world->defineRelation(name);
                break;
            case Property::ArrayPrimaryType:
                world->defineArrayProperty(name, (Property::ValueType)valueType);
                break;
            case Property::ArrayRelationType:
                world->defineArrayRelation(name);
                break;
            default:
                wlog("ignore invalid property of type: %d", type);
                break;
        }
	}
    if (errCode != SQLITE_DONE) {
        elog("error occurred in %s, errCode=%d", __func__, errCode);
    }
 
    sqlite3_reset(pstmt);
	sqlite3_finalize(pstmt);
    return errCode == SQLITE_DONE ? 0 : errCode;
}


int SqliteBeanDB::undefineProperty(const char* name)
{
    if (name == nullptr || name[0] == 0) return 0;
    if (m_db == nullptr) return -1;

    int errCode = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    static const char *sql = "DELETE FROM  PTABLE WHERE NAME = ?;";
    
	errCode = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, &pzTail);
    if (errCode != SQLITE_OK) return errCode;
    errCode = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (errCode != SQLITE_OK) return errCode;

	errCode = sqlite3_step( pstmt );
    if (errCode != SQLITE_DONE) {
        elog("error occurred in %s, errCode=%d", __func__, errCode);
    } else {
        errCode = 0;
    }
	sqlite3_finalize(pstmt);
    return errCode;
}


Property* SqliteBeanDB::defineProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::PrimaryType, valueType, needIndex);
}


Property* SqliteBeanDB::defineArrayProperty(const char* name, Property::ValueType valueType, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayPrimaryType, valueType, needIndex);
}


Property* SqliteBeanDB::defineRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::RelationType, Property::UIntType, needIndex);
}


Property* SqliteBeanDB::defineArrayRelation(const char* name, bool needIndex)
{
    return definePropertyCommon_(name, Property::ArrayRelationType, Property::UIntType, needIndex);
}


Property* SqliteBeanDB::definePropertyCommon_(const char* name, Property::Type type, 
    Property::ValueType valueType, bool needIndex)
{
    if (name == nullptr) return nullptr;
    if (name[0] == 0) return nullptr;

    static const char sql[] = "INSERT INTO PTABLE VALUES(?, ?, ?, ?) ;";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int errCode = 0;

	errCode = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, &pzTail);
    if (errCode != SQLITE_OK) return nullptr;

    errCode = sqlite3_bind_null(pstmt, 1);
    if (errCode != SQLITE_OK) goto out;
    errCode = sqlite3_bind_text(pstmt, 2, name, -1, nullptr);
    if (errCode != SQLITE_OK) goto out;
    errCode = sqlite3_bind_int(pstmt, 3, (int)type);
    if (errCode != SQLITE_OK) goto out;
    errCode = sqlite3_bind_int(pstmt, 4, (int)valueType);
    if (errCode != SQLITE_OK) goto out;
    
    errCode = sqlite3_step(pstmt);

out:
    if (errCode != SQLITE_OK && errCode != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return getProperty(name);
}


const Property* SqliteBeanDB::getProperty(const char* name) const
{
    return (const Property*) ((SqliteBeanDB*)this)->getProperty(name);
}


Property* SqliteBeanDB::getProperty(const char* name)
{
    if (name == nullptr || name[0] == 0) return nullptr;
    if (m_db == nullptr) return nullptr;
    
    BeanWorld *world = nullptr;
    if ((world = getWorld()) == nullptr) return nullptr;

    Property *property = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int errCode = 0;
    int type = 0;
    int valueType = 0;
    // int64_t id = 0;
   static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE FROM PTABLE WHERE NAME = ?;";

	errCode = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, &pzTail);
    if (errCode != SQLITE_OK) goto out;
    errCode = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (errCode != SQLITE_OK) goto out;

	while((errCode = sqlite3_step( pstmt )) == SQLITE_ROW) {
        nCol = 1;
        name = (const char*)sqlite3_column_text(pstmt, nCol++);
        type = sqlite3_column_int(pstmt, nCol++);
        valueType = sqlite3_column_int(pstmt, nCol++);
        switch (type) {
            case Property::PrimaryType:
                property = world->defineProperty(name, (Property::ValueType)valueType);
                break;
            case Property::RelationType:
                property = world->defineRelation(name);
                break;
            case Property::ArrayPrimaryType:
                property = world->defineArrayProperty(name, (Property::ValueType)valueType);
                break;
            case Property::ArrayRelationType:
                property = world->defineArrayRelation(name);
                break;
            default:
                wlog("ignore invalid property of type: %d", type);
                break;
        }
        break;
	}
    if (errCode != SQLITE_ROW) {
        elog("error occurred in %s, errCode=%d", __func__, errCode);
    }
 
out:
    sqlite3_reset(pstmt);
	sqlite3_finalize(pstmt);

    return property;
}


}
}
}