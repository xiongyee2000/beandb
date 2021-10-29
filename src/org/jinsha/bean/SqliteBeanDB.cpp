
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"
#include "./Bean.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <set>
#include <vector>
#include <sqlite3.h>
#include "jsoncpp/json/value.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"

#define DB_FNAME  "beandb.db"
#define PTABLE "PROPERTIES"
#define BTABLE "BEANS"
#define TTABLE "TRIPLES"

static const char* STR_INT = "INT";
static const char* STR_BIGINT = "BIGINT";
static const char* STR_INT8 = "INT8";
static const char* STR_REAL= "REAL";
static const char* STR_TEXT= "TEXT";

static const char* ERR_MSG_NOT_CONNECTED = "db not connected\n";

static void clean_stmt(sqlite3_stmt *pstmt)
{
    if (pstmt != nullptr) {
        sqlite3_clear_bindings(pstmt);
        sqlite3_reset(pstmt);
        sqlite3_finalize(pstmt);
    }
}


using namespace std;

namespace org {
namespace jinsha {
namespace bean {

static const char* CREATE_PTABLE =  
 "CREATE TABLE " PTABLE " ( \
ID INTEGER PRIMARY KEY NOT NULL, \
NAME VARCHAR(64) UNIQUE NOT NULL, \
PTYPE INT NOT NULL, \
VTYPE INT NOT NULL, \
INDEXED INT8 NOT NULL \
); ";

static const char* CREATE_BTABLE =  
 "CREATE TABLE " BTABLE " ( \
ID INTEGER PRIMARY KEY NOT NULL, \
VALUE TEXT DEFALT NULL,  \
NATIVE_DATA DEFALT NULL \
); ";

static const char* CREATE_TTABLE =  
 "CREATE TABLE " TTABLE " ( \
ID INTEGER PRIMARY KEY NOT NULL, \
SID INTEGER NOT NULL,  \
PID INTEGER NULL, \
OID INTEGER NULL \
); ";

static const char* CREATE_INDEX_TRIPLES_SP =
"CREATE INDEX INDEX_TRIPLES_SP \
ON " TTABLE "(SID, PID); ";

static const char* CREATE_INDEX_TRIPLES_PO =
"CREATE INDEX INDEX_TRIPLES_PO \
ON " TTABLE "(PID, OID); ";

static const char* CREATE_INDEX_TRIPLES_OS =
"CREATE INDEX INDEX_TRIPLES_OS \
ON " TTABLE "(OID, SID); ";

SqliteBeanDB::SqliteBeanDB( const char* dir) 
    : AbstractBeanDB()
    ,m_sqlite3Db_(nullptr)
    ,m_initialized_(false)
    ,m_transactionCount_(0)
    ,m_deletePropertyFromBeans_(false)
{
    int err = 0;
    if (dir == nullptr ||  dir[0] == 0) 
        m_dir_ = ".";
    else 
        m_dir_ = dir;
    m_dbFullPath_.append(m_dir_).append("/").append(DB_FNAME);
    err = checkDB();
    if (err == 0) {
        m_initialized_ = true;
    } else if (err == 1) {
        m_initialized_ = initDB() == 0 ? true : false;
    } else {
        m_initialized_ = false;
    }
}


SqliteBeanDB::~SqliteBeanDB()
{
    if (connected())
        disconnect();
}


int SqliteBeanDB::doConnect()
{
    if (!m_initialized_) return -1;
    if (m_sqlite3Db_ != nullptr) return 0;
    int err = 0;
    err = openDB();
    if (err) {
        elog("Failed to connect to db. err=%d", err);
    }
    return err;
}


int SqliteBeanDB::doDisconnect()
{
    if (!connected()) return 0;
    return closeDB();
}

int SqliteBeanDB::checkDB()
{
    if (m_dir_.empty()) return -1;
    if (access(m_dir_.c_str(), 0) != 0) return -1; //dir does not exist
    if (access(m_dir_.c_str(), 7)) return -2; //not enough permission on this dir

    if (access(m_dbFullPath_.c_str(), 0) != 0) return 1; //db file does not exist
    // if (access(m_dbFullPath_.c_str(), 6)) return 2; //not enough permission on the db file

    //todo: check database schema, data consistency etc. in future
    return 0;
}


int SqliteBeanDB::initDB()
{
    int err = 0;

    if (openDB()  !=  0) {
        elog("Failed to create database %s.\n", m_dbFullPath_.c_str());
        return -1;
    } 

    char *zErrMsg = nullptr;
    const char *sql = nullptr;

    sql = CREATE_PTABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Table " PTABLE " created successfully. \n");

    sql = CREATE_BTABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "Table " BTABLE " created successfully. \n");
    
    sql = CREATE_TTABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Table " TTABLE " created successfully. \n");

    sql = CREATE_INDEX_TRIPLES_SP;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_SP created successfully. \n");

    sql = CREATE_INDEX_TRIPLES_PO;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_PO created successfully. \n");

    sql = CREATE_INDEX_TRIPLES_OS;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_OS created successfully. \n");
    
    closeDB();
    return 0;
}


int SqliteBeanDB::clear() 
{
    return reInit();
}

int SqliteBeanDB::reInit()
{
    int err = 0;
    char buff[256] = {0};

    if (connected()) 
    {
        elog("%s", "Cannot do reinit when db is connected.");
        return -1;
    }

    if (access(m_dir_.c_str(), 0) != 0)  { //dir does not exist
        elog("Failed to reinit db for %s can not be accessed. \n",  m_dir_.c_str());
        return -1;
    }
    if (access(m_dir_.c_str(), 7)) { //not enough permission on this dir
        elog("Failed to reinit db for lack of write permission to %s. \n",  m_dir_.c_str());
        return -2; 
    }

    if (access(m_dbFullPath_.c_str(), 0) == 0)  //db file exists
    {
        // if (access(m_dbFullPath_.c_str(), 6)) { //not enough permission on the db file
        //     elog("Failed to reinit db for lack of permisson to write %s \n",  m_dbFullPath_.c_str());
        //     return 2;
        // }
        if (m_sqlite3Db_ != nullptr) {
            err = closeDB();
            if (err) {
                elog("%s", "Failed to reinit db for it cannot be closed. \n");
                return err;
            }
        }
        snprintf(buff, 256, "rm -rf \"%s\"", m_dbFullPath_.c_str() );
        err = system(buff);
        if (err) {
            elog("Failed to remove %s for reinit. (err=%d)\n",  m_dbFullPath_.c_str(), err);
            return err;
        }
    }

    err = initDB();
    m_initialized_ = err == 0 ? true : false;
    return err;
}


int SqliteBeanDB::openDB()
{
    int err = 0;
    if (m_sqlite3Db_ == nullptr)  {
        err = sqlite3_open(m_dbFullPath_.c_str(), &m_sqlite3Db_);
        if( err )  {
            elog("Failed to open database: %s\n", sqlite3_errmsg(m_sqlite3Db_));
            m_sqlite3Db_ = nullptr;
        } 
    }
    return err;
}

int SqliteBeanDB::closeDB()
{
    int err = 0;
    if (m_sqlite3Db_ == nullptr) return 0;
    err = sqlite3_close(m_sqlite3Db_);
    if( err )  {
        elog("Failed to close database: %s\n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        m_sqlite3Db_ = nullptr;
    }
    return err;
}


int SqliteBeanDB::createBean(oidType &beanId)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    static const char sql[] = "INSERT INTO " BTABLE " VALUES(?, ?, ?) ;";
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) return -1;

    err = sqlite3_bind_null(pstmt, 1);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_null(pstmt, 2);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_null(pstmt, 3);
    if (err != SQLITE_OK) goto out;

    err = sqlite3_step(pstmt);

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
        sqlite3_int64 id = sqlite3_last_insert_rowid(m_sqlite3Db_);
        beanId = (oidType)id;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;

}


int SqliteBeanDB::loadBeanBase(oidType beanId, Json::Value& value, Json::Value* nativeData) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    
    BeanWorld *world = getWorld();
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char SELECT_BEAN_1[] = "SELECT VALUE from " BTABLE " WHERE ID = ?;";
    static const char SELECT_BEAN_2[] = "SELECT VALUE, NATIVE_DATA from " BTABLE " WHERE ID = ?;";
    static const char SELECT_TRIPLE[] = "SELECT PID, OID from " TTABLE " WHERE SID = ?;";
    Json::Reader reader; 
    Json::Value jsonBean;  
    bool found = false;
    Json::Value* v = nullptr;
    oidType oid = 0;
    pidType pid = 0;

    if (nativeData == nullptr) {
        err = sqlite3_prepare_v2(m_sqlite3Db_, SELECT_BEAN_1, strlen(SELECT_BEAN_1), &pstmt, nullptr);
        if (err != SQLITE_OK) goto _out;
    } else {
        err = sqlite3_prepare_v2(m_sqlite3Db_, SELECT_BEAN_2, strlen(SELECT_BEAN_2), &pstmt, nullptr);
        if (err != SQLITE_OK) goto _out;
    }

    err = sqlite3_bind_int64(pstmt, 1, beanId);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        valueStr = (const char*)sqlite3_column_text(pstmt, 0);
        if (valueStr == nullptr) valueStr = "{}";
        if (!reader.parse(valueStr, value, false))
        {
            err = -1;
            elog("error parsing json string: %s", valueStr);
            break;
        }

        for (auto& pname : value.getMemberNames()) {
            property = world->getProperty(pname.c_str());
            if (property == nullptr) continue; //it's not a defined property
            //filter out delay load properties
            if (property->isDelayLoad()) {
                value[pname] = Json::Value::nullRef;
            } else {
                if (property->isArray()) {
                         //delay load, shall not reach here
                } else {
                    v = &value[pname];
                    switch (property->getValueType())
                    {
                    case Property::IntType:
                        (*v) = (*v).asInt64();
                         break;
                    case Property::UIntType:
                        (*v) = (*v).asUInt64();
                         break;
                    case Property::RealType:
                        (*v) = (*v).asDouble();
                         break;
                    case Property::BoolType:
                         //do nothing
                        break;
                    // case Property::StringType:
                    //     //delay load type
                    //     break;
                    default:
                        break;
                    }
                }
            }
        }

        //retrieve native data
        if (nativeData != nullptr) {
            if (sqlite3_column_type(pstmt, 1) != SQLITE_NULL) {
                valueStr = (const char*)sqlite3_column_text(pstmt, 1);
                if (valueStr == nullptr) valueStr = "{}";
                if (!reader.parse(valueStr, *nativeData, false))
                {
                    err = -2;
                    elog("error parsing json string: %s", valueStr);
                    goto _out;
                }
            } else {
                *nativeData = Json::Value::nullRef;
            }
        }
        
        found = true;
        err = 0;
        break;
	}

    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    pstmt = nullptr;

	err = sqlite3_prepare_v2(m_sqlite3Db_, SELECT_TRIPLE, strlen(SELECT_TRIPLE), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, 1, beanId);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        pid = sqlite3_column_int(pstmt, 0);
        oid = sqlite3_column_int64(pstmt, 1);
        property = world->getProperty(pid);
        // if (property == nullptr) continue; //shall not happen
        // if (!property->isRelation()) continue; //shall not happen
        pname = property->getName().c_str();
        if (property->isArray()) {
            if (!value[pname].isArray()) {
                value[pname] = Json::Value(Json::arrayValue);
            }
            value[pname].append(oid);
        } else {
            value[pname] = oid;
        }
    }

_out:
    if (err > 0) {
        err = handleErrAtEnd(err, false);
    }
    clean_stmt(pstmt);
    return err;
}


//  std::list<std::string> SqliteBeanDB::getBeanProperties_(oidType id) const
// {
//     std::list<std::string> properties;
//     sqlite3_stmt *pstmt = nullptr;
//     Property* property = nullptr;
//     sqlite3_int64 value = 0;
//     const char* valueStr = nullptr;
//     static const char sql[] = "SELECT VALUE from " BTABLE " WHERE ID = ?;";
//     Json::Reader reader; 
//     Json::Value jsonBean;
//     int err = 0;

// 	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
//     if (err != SQLITE_OK) goto _out;
//     err = sqlite3_bind_int64(pstmt, 1, id);
//     if (err != SQLITE_OK) goto _out;

// 	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
//         if (sqlite3_column_type(pstmt, 0) != SQLITE_NULL) {
//             valueStr = (const char*)sqlite3_column_text(pstmt, 0);
//             if (!reader.parse(valueStr, jsonBean))
//                 break;
//             for (auto& item : jsonBean.getMemberNames()) {
//                 properties.push_back(item);
//             }
//         }
//     }

// _out:
//     if (err != SQLITE_OK && err != SQLITE_DONE) {
//         elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
//     } else {
//         err = 0;
//     }
//     if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
//     sqlite3_reset(pstmt);
//     sqlite3_finalize(pstmt);
//     return properties;
// }


int  SqliteBeanDB::loadBeanProperty(oidType beanId, const Property* property, Json::Value& value)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    oidType id = 0; //id of the string in property table
    const char* pname = property->getName().c_str();
    char buff[128]{0};
    char * sql = &buff[0];
    sqlite3_stmt *pstmt = nullptr;
    Json::Value* v = nullptr;
    bool found = false;

    value = Json::Value::nullRef;

    Json::Value data;

    if (!property->isRelation()) {
        err = loadBeanBase(beanId, data);
        if (err)  {
            elog("Failed to load bean property (beanId=%llu, property name=%s) \n", beanId, pname);
            return err;
        }

        if (!data.isMember(pname)) {
            elog("Failed to load bean property for it does not contain the property (beanId=%llu, property name=%s) \n", beanId, pname);
            return -1001;
        }

        if (!property->isDelayLoad()) {
            value = data[pname];
            return 0;
        }
    }

    if (property->isRelation()) {
        snprintf(buff, sizeof(buff), "SELECT OID from " TTABLE " WHERE SID = ? AND PID = %d;", 
            property->getId());
    } else {
        snprintf(buff, sizeof(buff), "SELECT VALUE from p_%s WHERE SID = ?;", 
            pname);
    }

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, beanId);
    if (err != SQLITE_OK) goto out;

    while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        found = true;
        //do some preparation for array
        if (property->isArray())  {
                if (value.isNull()) value = Json::Value(Json::arrayValue); 
                value.append(Json::Value::nullRef);
                v = &value[value.size() - 1];
        } else {
            v = &value;
        }

        //set value according to value type
        switch (property->getValueType())
        {
        case Property::IntType:
#if defined(JSON_NO_INT64)
            (*v) = (Json::Int)sqlite3_column_int(pstmt, 0);
#else
            (*v) = (Json::Int64)sqlite3_column_int64(pstmt, 0);
#endif
            break;
        case Property::UIntType:
#if defined(JSON_NO_INT64)
            (*v) = (Json::UInt)sqlite3_column_int(pstmt, 0);
#else
            (*v) = (Json::UInt64)sqlite3_column_int64(pstmt, 0);
#endif
            break;
        case Property::RealType:
            (*v) =  sqlite3_column_double(pstmt, 0);
            break;
        case Property::BoolType:
            (*v) = sqlite3_column_int(pstmt, 0) == 0 ? false : true;
            break;
        case Property::StringType:
            (*v) = (const char*)sqlite3_column_text(pstmt, 0);
            break;
        default:
            //shall not reach here
            break;
        }
    }

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        value = Json::Value::nullRef;
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        if (found) {
            err = 0;
        } else {
            if (property->isRelation()) {
                 err = -1001;
                 value = Json::Value::nullRef;
                 ilog("Bean relation not found  (beanId=%lld, pname=%s) \n", beanId, property->getName().c_str());
            } else {
                if (property->isArray()) {
                    err = 0;
                    value = Json::Value(Json::arrayValue);
                    ilog("Bean's array property is empty  (beanId=%lld, pname=%s) \n", beanId, property->getName().c_str());
                } else {
                    if (property->isDelayLoad()) {
                        err = -1001;
                        value = Json::Value::nullRef;
                        ilog("Strange: Bean's property is missing  (beanId=%lld, pname=%s) \n", beanId, property->getName().c_str());
                    }
                }
            }
        }
    }
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::insertBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    if (property == nullptr) return -1;

    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    const char* pname = property->getName().c_str();
    char buff[128]{0};
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;
    Json::Value data;
    oidType sid = beanId;

    err = beginTransaction();
    if (err) {
        elog("Failed to insert bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isArray() && !property->isRelation()) {
        err = loadBeanBase(beanId, data);
        if (err) {
            elog("Failed in %s (beanId=%llu, property name=%s) \n ", __func__, beanId, pname);
            goto out;
        }

        if (data.isMember(pname)) {
            elog("Cannot insert an existing property (beanId=%llu, property name=%s) \n ", beanId, pname);
            goto out;
        } else {
            //add property to bean base record
            if (property->isDelayLoad())
            {
                data[pname] = Json::Value::nullRef;
            } else {
                data[pname] = value;
            }
            err = saveBeanBase(beanId, data);
            if (err) {
                elog("Failed to insert bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
                goto out;
            }
        }
    }

    if (property->isRelation()) {
        sql = "INSERT INTO " TTABLE "  (ID, SID, PID, OID) VALUES(?, ?, ?, ?) ;";
       err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
        if (err != SQLITE_OK) goto out;
        nCol = 1;
        err = sqlite3_bind_null(pstmt, nCol++);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)sid);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int(pstmt, nCol++, property->getId());
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)value.asInt64());
        if (err != SQLITE_OK) goto out;
        
        err = sqlite3_step(pstmt);
        if (err != SQLITE_OK && err != SQLITE_DONE) goto out;
    } else {
        snprintf(buff, sizeof(buff), "INSERT INTO p_%s  (ID, SID, VALUE) VALUES(?, ?, ?) ;", pname);
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
        if (err != SQLITE_OK) goto out;
        nCol = 1;
        err = sqlite3_bind_null(pstmt, nCol++);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)sid);
        if (err != SQLITE_OK) goto out;
        if (property->getValueType() == Property::StringType) {
            err = sqlite3_bind_text(pstmt, nCol++, value.asCString(), -1, nullptr);
        } else if (property->getValueType() == Property::RealType) {
            err = sqlite3_bind_double(pstmt, nCol++, value.asDouble());
        } else if (property->getValueType() == Property::BoolType) {
            err = sqlite3_bind_int(pstmt, nCol++, value.asBool() ? 1 : 0);
        } else {
            err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)value.asInt64());
        }
        if (err != SQLITE_OK) goto out;
        
        err = sqlite3_step(pstmt);
        if (err != SQLITE_OK && err != SQLITE_DONE) goto out;
    }

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::updateBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    if (property == nullptr) return -1;
    if (property->isArray()) return -1;
    return updateBeanProperty(beanId, property, (Json::ArrayIndex)-1, value);
}


int SqliteBeanDB::getIdByPropertyIndex(const Property* property, oidType sid, Json::ArrayIndex index, sqlite3_int64& id) const
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    Json::ArrayIndex i = 0;
    bool found = false;
    const char* pname = property->getName().c_str();
    pidType pid = property->getId();
    char buff[128]{0};
    char *sql = buff;

    if (property->isRelation()) {
        snprintf(buff, sizeof(buff), "SELECT ID FROM " TTABLE " WHERE SID = ? AND PID = %d ;", property->getId());
    } else {
        snprintf(buff, sizeof(buff), "SELECT ID FROM p_%s WHERE SID = ? ;", pname);
    }
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, sid);
    if (err != SQLITE_OK) goto out;

    while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        if (i == index) {
            id = sqlite3_column_int64(pstmt, 0);
            found = true;
            err = 0;
            break;
        }
        i++;
    }

    if (!found)
        err = -1001;

out:
    if (err == -1001) {
        //do nothing
    } else {
        err = ((SqliteBeanDB*)this)->handleErrAtEnd(err, false);
    }
    clean_stmt(pstmt);

    return err;
}


int SqliteBeanDB::updateBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    char buff[128]{0};
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int nCol = 0;
    oidType sid = beanId;
    pidType pid = 0;
    sqlite3_int64 id = 0;
    std::list<const Json::Value*> vlist;
    const Json::Value* v = nullptr;
    BeanWorld *world = nullptr;
   Json::Value data;
   const char* pname = nullptr;
   bool isArray = false;
   bool isRelation = false;

    if (property == nullptr) return -1;

    world = getWorld();
    if (world == nullptr) return -1;

    if (!property->isArray() && (index != (Json::Value::ArrayIndex)-1)) return -1;

   pname = property->getName().c_str();
   isArray = property->isArray();
   isRelation = property->isRelation();
   pid = property->getId();

    if (isArray) {
        err = getIdByPropertyIndex(property, sid, index, id);
        if (err) {
            elog("Failed to upate property (name=%s) for bean (id=%llu), for index (%u) is invalid \n", pname, beanId, index);
            return err;
        }
    }

    err = beginTransaction();
    if (err) {
        elog("Failed to update bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isDelayLoad() && !isRelation) {
        err = loadBeanBase(beanId, data);
        if (err) {
            elog("Failed in %s (beanId=%llu, property name=%s) \n ", __func__, beanId, pname);
            goto out;
        }

        if (!data.isMember(pname)) {
            elog("Cannot update a non-existing bean property. (beanId=%llu, property name=%s) \n ", beanId, pname);
            goto out;
        } else {
            //update property to bean base record
            data[pname] = value;
            err = saveBeanBase(beanId, data);
            if (err) {
                elog("Failed to update bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
                goto out;
            }
        }
    }

    v = &value;

    if (isRelation) {
        if (isArray) {
            sql = "UPDATE " TTABLE " SET OID = ? WHERE ID = ?  ;";
        } else {
            sql =  "UPDATE " TTABLE " SET OID = ? WHERE SID = ? AND PID = ?  ;";
        }
    } else {
        if (isArray) {
            snprintf(buff, sizeof(buff), "UPDATE p_%s SET VALUE = ? WHERE ID = ?  ;", pname);
        } else {
            snprintf(buff, sizeof(buff), "UPDATE p_%s SET VALUE = ? WHERE SID = ?  ;", pname);
        }
    }

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    nCol = 1;
    if (property->getValueType() == Property::StringType) {
        err = sqlite3_bind_text(pstmt, nCol++, v->asCString(), -1, nullptr);
    } else if (property->getValueType() == Property::RealType) {
        err = sqlite3_bind_double(pstmt, nCol++, v->asDouble());
    } else if (property->getValueType() == Property::BoolType) {
        err = sqlite3_bind_int(pstmt, nCol++, v->asBool() ? 1 : 0);
    } else {
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)v->asInt64());
    }
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, nCol++, isArray ? (sqlite3_int64)id : (sqlite3_int64)sid);
    if (err != SQLITE_OK) goto out;

    if (isRelation && !isArray) {
        err = sqlite3_bind_int(pstmt, nCol++, pid);
        if (err != SQLITE_OK) goto out;
    }

    err = sqlite3_step(pstmt);

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);

    return err;
}

int SqliteBeanDB::deleteBeanProperty(oidType beanId, 
    const Property* property) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    if (property == nullptr) return -1;

    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    bool isArray = property->isArray();
    char buff[128]{0};
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;
    Json::Value data;
    oidType sid = beanId;
    const char* pname = property->getName().c_str();

    if (!property->isRelation()) {
        err = loadBeanBase(beanId, data);
        if (err) {
            elog("Failed to delete bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
            goto out;
        }
    }

    err = beginTransaction();
    if (err) {
        elog("Failed to delete bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isRelation()) {
        if (data.isMember(pname)) {
            data.removeMember(pname);
            err = saveBeanBase(beanId, data);
            if (err) {
                elog("Failed to insert bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
                goto out;
            }
        } else {
            wlog("The bean has no such property to delete (beanId=%llu, property name=%s) \n ", beanId, pname);
            goto out;
        }
    }

    if (property->isRelation()) {
        sql = "DELETE FROM " TTABLE " WHERE SID = ?;";
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
        if (err != SQLITE_OK) goto out;
        nCol = 1;
        err = sqlite3_bind_int64(pstmt, nCol++, sid);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_step(pstmt);
        if (err != SQLITE_OK) goto out;
    } else {
        if (!m_deletePropertyFromBeans_) {
            snprintf(buff, sizeof(buff), "DELETE FROM p_%s WHERE SID = ?;", pname);
            err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
            if (err != SQLITE_OK) goto out;
            nCol = 1;
            err = sqlite3_bind_int64(pstmt, nCol++, sid);
            if (err != SQLITE_OK) goto out;
            err = sqlite3_step(pstmt);
            if (err != SQLITE_OK) goto out;
        }
    }

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);

    return err;
}


int SqliteBeanDB::deleteBeanProperty(oidType beanId, 
    const Property* property, 
    Json::Value::ArrayIndex index) 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    if (property == nullptr) return -1;

    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    bool isArray = property->isArray();
    if (!isArray) return -1;

    char buff[128]{0};
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;
    Json::Value data, nativeData;
    oidType sid = beanId;
    sqlite3_int64 id = 0;
    const char* pname = property->getName().c_str();

    err = getIdByPropertyIndex(property, beanId, index, id);
    if (err == 0) {
        //found the record, but do nothing here
    } else if (err == -1001) {
        wlog("Take %s as succeeded for no such record is found (beanId=%llu, property name=%s, index=%llu) \n", __func__, beanId, pname, index);
        return 0;
    } else {
        //unexpected exceptions
        return err;
    }

    err = beginTransaction();
    if (err) {
        elog("Failed to delete bean property (beanId=%llu, property name=%s, index=%llu) \n", beanId, pname, index);
        return err;
    }

    if (property->isRelation()) 
        snprintf(buff, sizeof(buff), "DELETE FROM " TTABLE " WHERE ID = ?;");
    else
        snprintf(buff, sizeof(buff), "DELETE FROM p_%s WHERE ID = ?;", pname);
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    nCol = 1;
    err = sqlite3_bind_int64(pstmt, nCol++,  (sqlite3_int64)id);
    if (err != SQLITE_OK) goto out;

    err = sqlite3_step(pstmt);

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);

    return err;
}


int SqliteBeanDB::deleteBean(oidType id)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    static const char *sql = "DELETE FROM  " BTABLE " WHERE ID = ?;";

    err = beginTransaction();
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, (sqlite3_int64)id);
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );
    if (err != SQLITE_DONE) goto out;

    err = deleteRelationByObject(id);
    if (err != SQLITE_DONE) goto out;

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);

    return err;
}


int SqliteBeanDB::loadProperties(std::unordered_map<std::string, Property*>& properties) const
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE, INDEXED FROM " PTABLE ";";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int err = 0;
    int type = 0;
    int valueType = 0;
    bool indexed = false;
    const char *name = nullptr;
    int id = 0;
    bool delayLoad = false;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        nCol = 0;
        id = sqlite3_column_int(pstmt, nCol++);
        name = (const char*)sqlite3_column_text(pstmt, nCol++);
        type = sqlite3_column_int(pstmt, nCol++);
        valueType = sqlite3_column_int(pstmt, nCol++);
        indexed = (sqlite3_column_int(pstmt, nCol++) == 1 ? true : false);
        delayLoad = determineDelayLoad((Property::Type)type, (Property::ValueType)valueType);
        Property*  property = newProperty(name,  
            (pidType)id, 
            (Property::Type)type, 
            (Property::ValueType)valueType, 
            delayLoad);
        assert(property);
        properties[name] = property;
	}
    if (err == SQLITE_DONE) err = SQLITE_OK; 
 
 out:
    err =  ((SqliteBeanDB*)this)->handleErrAtEnd(err, false);
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::undefineProperty(Property* property)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    if (property == nullptr) return 0;

    int err = 0;
    const char* name = property->getName().c_str();
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* errMsg = nullptr;
    char buff[256]{0};
    int buffSize = sizeof(buff);
    static const char *sql_delete_property = "DELETE FROM  " PTABLE " WHERE NAME = ?;";
    static const char *sql_delete_rtable = "DELETE FROM  " TTABLE " WHERE PID = %d;";
    static const char drop_ptable[] =   "DROP TABLE p_%s; ";
    // static const char drop_index[] =   "DROP INDEX p_%s_index; ";

    err = beginTransaction();
    if (err) {
        elog("Failed to undefine property (name =%s) \n",  name);
        return err;
    }

    err = deletePropertyFromAllBeans(property);
    if (err != 0) goto _out;

    if (property->isRelation()) {
        snprintf(buff, buffSize, sql_delete_rtable, property->getId());
        err = sqlite3_exec(m_sqlite3Db_, buff, nullptr , nullptr , &errMsg );
        if (err != SQLITE_OK) goto _out;
    } else {
        snprintf(buff, buffSize, drop_ptable, name);
        err = sqlite3_exec(m_sqlite3Db_, buff, nullptr , nullptr , &errMsg );
        if (err != SQLITE_OK) goto _out;
    }
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql_delete_property, strlen(sql_delete_property), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (err != SQLITE_OK) goto _out;
	err = sqlite3_step( pstmt );
    if (err != SQLITE_DONE) goto _out;

_out:
    err = handleErrAtEnd(err, true);
    if (errMsg  != nullptr) sqlite3_free(errMsg);
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::defineProperty(const char* name, 
    Property::Type type, 
    Property::ValueType valueType, 
    pidType& pid,
    bool& delayLoad)
{
    if (name == nullptr || name[0] == 0) return -1;

     //now let's create it in db
    if (m_sqlite3Db_ == nullptr) return -2;

    const char* sql = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    const char* vtype_str = nullptr;
    char* errMsg = nullptr;
    char  buff[256] {0};
    int sizeOfBuff = sizeof(buff);
    static const char create_property_table[] =   
    "CREATE TABLE p_%s ( \
    ID INTEGER PRIMARY KEY NOT NULL, \
    SID BIGINT %s NOT NULL, \
    VALUE  %s NOT NULL \
    ); ";
    static const char create_property_index[] =   
    "CREATE INDEX index_p_%s \
    ON p_%s(VALUE); ";
    static const char sql_insert_ptable[] = "INSERT INTO " PTABLE " VALUES(?, ?, ?, ?, ?) ;";

    bool isRelation = ((type == Property::RelationType || 
        type == Property::ArrayRelationType) ? true : false);
    bool isArray = ((type == Property::ArrayPrimaryType || 
        type == Property::ArrayRelationType) ? true : false);

    err = beginTransaction();
    if (err) {
        elog("Failed to define property (name =%s) \n", name);
        return err;
    }

    sql = sql_insert_ptable;
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) return -3;

    err = sqlite3_bind_null(pstmt, 1);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_text(pstmt, 2, name, -1, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int(pstmt, 3, (int)type);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int(pstmt, 4, (int)valueType);
    if (err != SQLITE_OK) goto _out;
    // err = sqlite3_bind_int(pstmt, 5, needIndex ? 1 : 0);
    err = sqlite3_bind_int(pstmt, 5, 1);
    if (err != SQLITE_OK) goto _out;
    
    err = sqlite3_step(pstmt);
    if (err != SQLITE_DONE) goto _out;

    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    pstmt = nullptr;

    if (!isRelation) {
        //create property table
        switch (valueType)
        {
        case Property::BoolType:
            vtype_str = STR_INT8;
            break;
        case Property::RealType:
            vtype_str = STR_REAL;
            break;
        case Property::StringType:
            vtype_str = STR_TEXT;
        default:
            vtype_str = STR_BIGINT;
            break;
        }
        snprintf(buff, sizeOfBuff, create_property_table, 
        name,
        isArray ? "" : "UNIQUE",
        vtype_str);
        sql = buff;
        err = sqlite3_exec ( m_sqlite3Db_ , sql , nullptr , nullptr , &errMsg );
        if (err != SQLITE_OK)  goto _out;
        snprintf(buff, sizeOfBuff, create_property_index, name, name);
        err = sqlite3_exec ( m_sqlite3Db_ , sql , nullptr , nullptr , &errMsg );
        if (err != SQLITE_OK)  goto _out;
    }

    delayLoad = determineDelayLoad(type, valueType);

_out:
    err = handleErrAtEnd(err, true);
    if (!err) {
        pid = (pidType)sqlite3_last_insert_rowid(m_sqlite3Db_);
    }
    clean_stmt(pstmt);
    if (errMsg  != nullptr) sqlite3_free(errMsg);

    return err;
}


int SqliteBeanDB::beginTransaction()
{
    int err = 0;
    if (m_transactionCount_ == 0) {
        err = doBeginTransaction();
        if (err)  {
            elog("Failed to begin transaction (err=%d) \n", err);
        }  else {
            m_transactionCount_ = 1;
        }
    } else {
        m_transactionCount_++;
    }
    return err;
}


int SqliteBeanDB::commitTransaction()
{
    int err = 0;
    if (m_transactionCount_ > 0) {
        m_transactionCount_--;
        if (m_transactionCount_ == 0) {
            err = doCommitTransaction();
            if (err)  {
                elog("%s", "Failed to commit transaction\n");
                m_transactionCount_ = 1;
            }
        }
    } else {
        elog("%s", "Not in transaction so cannot do commit.\n");
        err = -1;
    }
    return err;
}


int SqliteBeanDB::rollbackTransaction()
{
    int err = 0;
    if (m_transactionCount_ > 0) {
        m_transactionCount_--;
        if (m_transactionCount_ == 0) {
            err = doRollbackTransaction();
            if (err)  {
                elog("%s", "Failed to rollback transaction\n");
                m_transactionCount_ = 1;
            }
        }
    } else {
        elog("%s", "Not in transaction so cannot do rollback.\n");
        err = -1;
    }
    return err;
}


bool SqliteBeanDB::inTransaction()
{
    return m_transactionCount_ > 0;
}


int SqliteBeanDB::doBeginTransaction() 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::doCommitTransaction() 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::doRollbackTransaction() 
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "ROLLBACK TRANSACTION", nullptr, nullptr, &errMsg);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::saveBeanBase(oidType beanId, const Json::Value& data, const Json::Value* nativeData)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    if (data.isNull()) return -2;

    int err = 0;
    sqlite3_int64 beanId_ = (sqlite3_int64)beanId;
    char* errMsg = nullptr;
    Json::Reader reader;
    Json::FastWriter jsonWriter;
    Json::Value tmpValue = Json::Value(Json::objectValue);
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* tmpStr = nullptr;
     std::string valueStr;
     const Property* property = nullptr;
     int nCol = 0;
    static const char *sql_1 = "UPDATE " BTABLE " SET  VALUE = ? WHERE ID = ?;";
    static const char *sql_2 = "UPDATE " BTABLE " SET  VALUE = ?, NATIVE_DATA = ? WHERE ID = ?;";

    if (nativeData == nullptr) {
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql_1, strlen(sql_1), &pstmt,nullptr);
        if (err != SQLITE_OK) goto _out;
    } else {
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql_2, strlen(sql_2), &pstmt,nullptr);
        if (err != SQLITE_OK) goto _out;
    }

    for (auto& pname : data.getMemberNames()) {
        property = getWorld()->getProperty(pname.c_str());
        if (property == nullptr) continue; //todo: error here?
        if (property->isRelation()) continue; //do not save relation info.
        if (!property->isDelayLoad()) {
            tmpValue[pname] = data[pname];
        } else {
            tmpValue[pname] = Json::Value::nullRef;
        }
    }

    valueStr = jsonWriter.write(tmpValue);
    tmpStr = sqlite3_mprintf("%q", valueStr.c_str());
    nCol = 1;
    err = sqlite3_bind_text(pstmt, nCol++, tmpStr , -1, nullptr);
    if (err != SQLITE_OK) goto _out;

    if (nativeData != nullptr) {
        if (nativeData->isNull()) {
            err = sqlite3_bind_null(pstmt, nCol++);
        }  else {
            std::string nativeDataStr = jsonWriter.write(*nativeData);
            tmpStr = sqlite3_mprintf("%q", nativeDataStr.c_str());
            err = sqlite3_bind_text(pstmt, nCol++, tmpStr , -1, nullptr);
        }
        if (err != SQLITE_OK) goto _out;
    }

    err = sqlite3_bind_int64(pstmt, nCol++, beanId_);
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_step(pstmt);
    if (err != SQLITE_OK) goto _out;

_out:
    err = handleErrAtEnd(err, false);
    clean_stmt(pstmt);
    return err; 
}

int SqliteBeanDB::loadBeanNativeData(oidType beanId, Json::Value& data)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    
    BeanWorld *world = nullptr;
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT NATIVE_DATA from " BTABLE " WHERE ID = ?;";
    Json::Reader reader; 
    Json::Value jsonBean;  
    bool found = false;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, 1, beanId);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        //retrieve native data
        if (sqlite3_column_type(pstmt, 0) != SQLITE_NULL) {
            valueStr = (const char*)sqlite3_column_text(pstmt, 0);
            if (valueStr == nullptr) {
                // data = Json::Value(Json::nullValue); //can reach here?
            } else {
                if (!reader.parse(valueStr, data, false))
                {
                    err = -2;
                    elog("error parsing json string: %s \n", valueStr);
                    break;
                }
            }
        } else {
            data = Json::Value::nullRef;
        }
        
        found = true;
        err = 0;
        break;
	}

_out:
    if (err >= 0) {
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
            elog("Failed in %s (err=%d) \n", __func__, err);
        } else {
            if (!found)
                err = -1001;
        }
    } else { //err < 0
        elog("Failed in %s (err=%d) \n", __func__, err);
    }
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::updateBeanNativeData(oidType beanId, 
    const Json::Value& nativeData)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    char* errMsg = nullptr;
    Json::Reader reader;
    Json::FastWriter jsonWriter;
    Json::Value tmpValue;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    std::string valueStr;
    char* tmpStr = nullptr;
    static const char *sql = "UPDATE " BTABLE " SET  NATIVE_DATA = ? WHERE ID = ?;";

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;

    if (!nativeData.isNull()) {
        valueStr = jsonWriter.write(nativeData);
        tmpStr = sqlite3_mprintf("%q", valueStr.c_str());
    }
    if (tmpStr == nullptr) {
        err = sqlite3_bind_null(pstmt, 1);
    } else {
        err = sqlite3_bind_text(pstmt, 1, tmpStr , -1, nullptr);
    }
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_bind_int64(pstmt, 2, (sqlite3_int64)beanId);
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_step(pstmt);
    if (err != SQLITE_OK) goto _out;

_out:
    err = handleErrAtEnd(err, false);
    clean_stmt(pstmt);
    return err; 
}

int SqliteBeanDB::deleteBeanNativeData(oidType beanId)
{
    if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;
    Json::Value value = Json::Value(Json::nullValue);
    return updateBeanNativeData(beanId, value);
}

bool SqliteBeanDB::determineDelayLoad(Property::Type type, Property::ValueType valueType)
{
    bool delayLoad = false;
    if (type == Property::RelationType || 
         type == Property::ArrayRelationType) {
        delayLoad = false;
    } else if (type == Property::PrimaryType) {
        if (valueType == Property::StringType) 
            delayLoad = true;
        else {
            delayLoad = false;
        }
    } else { //type == Property::ArrayPrimaryType
        delayLoad = true;
    }
    return delayLoad;
}

int SqliteBeanDB::deleteRelationByObject(oidType id)
{
    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    static const char *sql = "DELETE FROM  " TTABLE " WHERE OID = ?;";
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, id);
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );

out:
    err = handleErrAtEnd(err, false);
    clean_stmt(pstmt);
    return err;
}

int SqliteBeanDB::deletePropertyFromAllBeans(Property* property)
{
    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    const char* pname = property->getName().c_str();
    static const char SELECT_PTABLE[] = "SELECT SID from p_%s";
    static const char SELECT_TRIPLE[] = "SELECT SID from " TTABLE " WHERE PID = %d;";
    char buff[128]{0};
    const char* sql = buff;
    const int COUNT = 128; //todo: what should be the best optimzed number?
    oidType sid = 0;
    std::set<oidType> sids;
    bool finished = false;

    err = beginTransaction();
    if (err) {
        elog("Failed in deletePropertyFromAllBeans() (property name=%s) \n", pname);
        return err;
    }

    if (property->isRelation()) {
        snprintf(buff, sizeof(buff), SELECT_TRIPLE, property->getId());
    } else {
        snprintf(buff, sizeof(buff), SELECT_PTABLE, pname);
    }

    while (!finished) {
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
        if (err != SQLITE_OK) goto out;

        sids.clear();
        while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
            sid = sqlite3_column_int64(pstmt, 0);
            sids.insert(sid);
            if (sids.size() >= COUNT) break;
        }
        if (err != SQLITE_ROW) finished = true;

        sqlite3_clear_bindings(pstmt);
        sqlite3_reset(pstmt);
        sqlite3_finalize(pstmt);
        pstmt = nullptr;
        
        //delete bean property
        m_deletePropertyFromBeans_ = true;
        for (auto& item : sids) {
            err = deleteBeanProperty(item, property);
            if (err) goto out;
        }
        m_deletePropertyFromBeans_ = false;

    }

out:
    err = handleErrAtEnd(err, true);
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::loadPage_findBeans(opType optype, const Property* property, const Json::Value& value, BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids)
{
   if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    static const char* OP_EQ_STR = "==";
    static const char* OP_LE_STR = "<=";
    static const char* OP_LT_STR = "<";
    static const char* OP_GE_STR = ">=";
    static const char* OP_GT_STR = ">";
    static const char* OP_LIKE_STR = "like";
    int err = 0;
    bool found = false;
    char buff[128]{0};
    char * sql = &buff[0];
    const char* op_str = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int size = 0;
    int nCol = 1;
    oidType sid = 0;
    oidType oid = 0;
    sqlite3_int64 limitFrom = pageSize * pageIndex;
    const auto& pname = property->getName().c_str();
    pidType pid = property->getId();
    bool isRelation = property->isRelation();
    bool isArray = property->isArray();
    Property::Type ptype = property->getType();
    Property::ValueType vtype = property->getValueType();
    SqliteBeanIdPage* sqlitePage = (SqliteBeanIdPage*)page;

    switch (optype)
    {
        case op_eq:
            op_str = OP_EQ_STR;
            break;
        case op_le:
            op_str = OP_LE_STR;
            break;
        case op_lt:
            op_str = OP_LT_STR;
            break;
        case op_ge:
            op_str = OP_GE_STR;
            break;
        case op_gt:
            op_str = OP_GT_STR;
            break;
        case op_like:
            if (value.asString().empty()) return -1;
            op_str = OP_LIKE_STR;
            break;
        default:
            err = -1;
            goto _out;
            break;
    }

    if (isRelation) {
        snprintf(buff, sizeof(buff), "SELECT DISTINCT SID from " TTABLE " WHERE PID = ? AND OID %s ? limit ?,? ;", op_str);
    } else {
        snprintf(buff, sizeof(buff), "SELECT DISTINCT SID from p_%s WHERE VALUE %s ? limit ?,? ;", pname, op_str);
    }
    
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    if (isRelation) {
        err = sqlite3_bind_int64(pstmt, nCol++, pid);
        if (err != SQLITE_OK) goto _out;
#if defined(JSON_NO_INT64)
            oid = (oidType)value.asInt(), 
#else
            oid = (oidType)value.asInt64(), 
#endif
        err = sqlite3_bind_int64(pstmt, nCol++, oid);
        if (err != SQLITE_OK) goto _out;
    } else {
        switch (value.type()) {
            case Json::intValue:
                err = sqlite3_bind_int(pstmt, nCol++, value.asInt());
                break;
            case Json::uintValue:
                err = sqlite3_bind_int(pstmt, nCol++, value.asUInt());
                break;
            case Json::realValue:
                err = sqlite3_bind_double(pstmt, nCol++, value.asDouble());         
                break;
            case Json::stringValue:
                err = sqlite3_bind_text(pstmt, nCol++, value.asCString(), -1, nullptr);         
                break;
            case Json::booleanValue:
                err = sqlite3_bind_int(pstmt, nCol++, value.asBool() ? 1 : 0);
                break;
            default:
                elog("[%s:%d] value type %d not supported" ,  __FILE__, __LINE__, value.type());
                break;
        }
        if (err != SQLITE_OK) goto _out;      
    }

    err = sqlite3_bind_int64(pstmt, nCol++, limitFrom);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int(pstmt, nCol++, pageSize);
    if (err != SQLITE_OK) goto _out;

    sids.clear();
	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        sid = sqlite3_column_int64(pstmt, 0);
        sids.push_back(sid);
        found = true;
    }

_out:
    if (err > 0) {
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        } else {
            if (found)
                err = 0;
            else 
                err = -1001;
        }
    }
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::loadPage_findSubjects_Objects(bool findSubjects, const Property* property, BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids)
{
   if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    bool found = false;
    char buff[128]{0};
    char * sql = &buff[0];
    const char* op_str = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int size = 0;
    int nCol = 1;
    oidType sid = 0;
    sqlite3_int64 limitFrom = pageSize * pageIndex;
    const auto& pname = property->getName().c_str();
    pidType pid = property->getId();
    bool isRelation = property->isRelation();
    bool isArray = property->isArray();
    Property::Type ptype = property->getType();
    Property::ValueType vtype = property->getValueType();

    if (findSubjects) 
        snprintf(buff, sizeof(buff), "SELECT DISTINCT SID from " TTABLE " WHERE PID = ? limit ?,? ;");
    else
        snprintf(buff, sizeof(buff), "SELECT DISTINCT OID from " TTABLE " WHERE PID = ? limit ?,? ;");
    
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, nCol++, pid);
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_bind_int64(pstmt, nCol++, limitFrom);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int(pstmt, nCol++, pageSize);
    if (err != SQLITE_OK) goto _out;

    sids.clear();
	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        sid = sqlite3_column_int64(pstmt, 0);
        sids.push_back(sid);
        found = true;
    }

_out:
    if (err > 0) {
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        } else {
            if (found)
                err = 0;
            else 
                err = -1001;
        }
    }
    clean_stmt(pstmt);
    return err;
}


int SqliteBeanDB::loadPage_getAllBeans(BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids)
{
   if (0 != checkConnected(ERR_MSG_NOT_CONNECTED)) return -1;

    int err = 0;
    bool found = false;
    static char * sql = "SELECT ID from " BTABLE " limit ?,? ;";
    sqlite3_stmt *pstmt = nullptr;
    int size = 0;
    int nCol = 1;
    oidType id = 0;
    sqlite3_int64 limitFrom = pageSize * pageIndex;

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_bind_int64(pstmt, nCol++, limitFrom);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int(pstmt, nCol++, pageSize);
    if (err != SQLITE_OK) goto _out;

    sids.clear();
	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        id = sqlite3_column_int64(pstmt, 0);
        sids.push_back(id);
        found = true;
    }

_out:
    if (err > 0) {
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        } else {
            if (found)
                err = 0;
            else 
                err = -1001;
        }
    }
    clean_stmt(pstmt);
    return err;
}


BeanIdPage* SqliteBeanDB::findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize) const
{  
    if (optype == op_has) return nullptr;
    if (property == nullptr) return nullptr; 
    if (value.isNull()) return nullptr; 
    if (value.isArray()) return nullptr; 
    if (value.isObject()) return nullptr; 
    if (pageSize == 0) return nullptr;
    if (!connected()) return nullptr;

    int err = 0;
    SqliteBeanIdPage* page = nullptr;
    auto& pname = property->getName();
    pidType pid = property->getId();
    bool isRelation = property->isRelation();
    bool isArray = property->isArray();
    Property::Type ptype = property->getType();
    Property::ValueType vtype = property->getValueType();

    if (vtype != (Property::ValueType)value.type()) return nullptr;

    if (isRelation && optype != op_eq) {
        elog("%s", "relation search only supports op_eq. \n");
        return nullptr;
    }

    if (vtype == Property::BoolType && optype != op_eq) {
        elog("%s", "bool value only supports op_eq. \n");
        return nullptr;
    }

    if (vtype != Property::StringType && optype == op_like) {
        elog("%s", "op_like only applies to string type. \n");
        return nullptr;
    }

    auto func = std::bind(&SqliteBeanDB::loadPage_findBeans, 
        (SqliteBeanDB*)this, 
        optype,
        property, 
        value,
        placeholders::_1,
        placeholders::_2, 
        placeholders::_3,
        placeholders::_4);
    page = new SqliteBeanIdPage(pageSize, func);

    return page;
}


BeanIdPage* SqliteBeanDB::findSubjects(const Property* property, unsigned int pageSize) const
{
    return findSubjectsObjects(true, property, pageSize);
}


BeanIdPage* SqliteBeanDB::findObjects(const Property* property, unsigned int pageSize) const
{
    return findSubjectsObjects(false, property, pageSize);
}


BeanIdPage* SqliteBeanDB::findSubjectsObjects(bool findSubjects, const Property* property, unsigned int pageSize) const
{
    if (property == nullptr) return nullptr; 
    if (pageSize == 0) return nullptr;
    if (!connected()) return nullptr;

    int err = 0;
    SqliteBeanIdPage* page = nullptr;
    auto& pname = property->getName();
    pidType pid = property->getId();
    bool isRelation = property->isRelation();

    if (!isRelation) {
        elog("%s", "only relation property is supported for this method. \n");
        return nullptr;
    }

    auto func = std::bind(&SqliteBeanDB::loadPage_findSubjects_Objects, 
        (SqliteBeanDB*)this, 
        findSubjects,
        property, 
        placeholders::_1, 
        placeholders::_2,
        placeholders::_3,
        placeholders::_4);
    page = new SqliteBeanIdPage(pageSize, func);

    return page;
}

BeanIdPage* SqliteBeanDB::getAllBeans(unsigned int pageSize) const
{
    if (!connected()) return nullptr;
    auto func = std::bind(&SqliteBeanDB::loadPage_getAllBeans, 
        (SqliteBeanDB*)this, 
        placeholders::_1, 
        placeholders::_2,
        placeholders::_3,
        placeholders::_4);

    BeanIdPage* page = new SqliteBeanIdPage(pageSize, func);

    return page;
}


int SqliteBeanDB::checkConnected(const char* errMsg) const
{
        if (!connected()) {                  
            if (errMsg != nullptr)                      
            elog("%s", errMsg); 
            return -1;                                                    
        }                       
        return 0;
}


int SqliteBeanDB::handleErrAtEnd(int err, bool finishTransaction)
{
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        if (finishTransaction) {
            rollbackTransaction();
        }
    } else {
        if (finishTransaction) {
            err = commitTransaction();
            if (err) {
                rollbackTransaction();
            }
        } else {
            err = 0;
        }
    }
    return err;
}

}
}
}