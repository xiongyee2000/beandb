
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"
#include "./Bean.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include "jsoncpp/json/value.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"

#define CHECK_CONNECTED()                     \
    do {                                                                        \
        if (!connected()) {                                        \
            elog("%s", "db not connected\n"); \
            return -1;                                                    \
        }                                                                          \
    } while (0);

#define SMART_ROLLBACK()                         \
        do {                                                                    \
        if (!alreadyInTransaction) {                     \
                rollbackTransaction();                      \
        }                                                                          \
    } while (0);

#define SMART_COMMIT()                              \
    do {                                                                        \
        if (!alreadyInTransaction) {                     \
            err = commitTransaction();                \
        }                                                                         \
    } while (0);

#define SMART_BEGIN_TRANSACTION()             \
    do {                                                                                   \
        alreadyInTransaction = inTransaction();       \
        if (!alreadyInTransaction) {                                 \
            err = beginTransaction();                                 \
        };                                                                                    \
    } while (0);

static const char* STR_INT = "INT";
static const char* STR_BIGINT = "BIGINT";
static const char* STR_INT8 = "INT8";
static const char* STR_REAL= "REAL";
static const char* STR_TEXT= "TEXT";

namespace org {
namespace jinsha {
namespace bean {

const char* SqliteBeanDB::DB_PATH  =  "beandb.db";
// const char* SqliteBeanDB::PTABLE_NAME = "PTABLE";
// const char* SqliteBeanDB::OTABLE_NAME = "BEAN";

static const char* CREATE_PROPERTY_TABLE =  
 "CREATE TABLE PROPERTY ( \
ID INTEGER PRIMARY KEY NOT NULL, \
NAME VARCHAR(64) UNIQUE NOT NULL, \
PTYPE INT NOT NULL, \
VTYPE INT NOT NULL, \
INDEXED INT8 NOT NULL \
); ";

static const char* CREATE_BEAN_TABLE =  
 "CREATE TABLE BEAN ( \
ID INTEGER PRIMARY KEY NOT NULL, \
VALUE TEXT DEFALT NULL,  \
NATIVE_DATA DEFALT NULL \
); ";

static const char* CREATE_TRIPLE_TABLE =  
 "CREATE TABLE TRIPLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
SID INTEGER NOT NULL,  \
PID INTEGER NULL, \
OID INTEGER NULL \
); ";

static const char* CREATE_TRIPLE_INDEX_SP =
"CREATE INDEX TRIPLE_SP \
ON TRIPLE(SID, PID); ";

static const char* CREATE_TRIPLE_INDEX_PO =
"CREATE INDEX TRIPLE_PO \
ON TRIPLE(PID, OID); ";

static const char* CREATE_TRIPLE_INDEX_OS =
"CREATE INDEX TRIPLE_OS \
ON TRIPLE(OID, SID); ";

SqliteBeanDB::SqliteBeanDB( const char* dir) 
    : AbstractBeanDB()
    , m_dir(dir)
    ,m_sqlite3Db_(nullptr)
    ,m_initialized(false)
{
    if (dir == nullptr ||  dir[0] == 0) return;
    m_dbFullPath.append(m_dir).append("/").append(DB_PATH);
    if (checkDB() >= 0)
        m_initialized = internalInit() == 0 ? true : false;
}


SqliteBeanDB::~SqliteBeanDB()
{
    if (connected())
        disconnect();
}


int SqliteBeanDB::connect_()
{
    if (!m_initialized) return -1;
    if (m_sqlite3Db_ != nullptr) return 0;
    int err = 0;
    err = openDB();
    if (err) {
        elog("Failed to connect to db. err=%d", err);
    }
    return err;
}


int SqliteBeanDB::disconnect_()
{
    if (!connected()) return 0;
    return closeDB();
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
    int err = 0;

    if (access(m_dbFullPath.c_str(), 0) == 0) return 0; //db file exists

    if (openDB()  !=  0) {
        elog("Failed to create database %s.\n", m_dbFullPath.c_str());
        return -1;
    } 

    char *zErrMsg = nullptr;
    const char *sql = nullptr;

    sql = CREATE_PROPERTY_TABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Table PROPERTY created successfully. \n");

    sql = CREATE_BEAN_TABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "Table BEAN created successfully. \n");
    
    sql = CREATE_TRIPLE_TABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Table TRIPLE created successfully. \n");

    sql = CREATE_TRIPLE_INDEX_SP;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_SP created successfully. \n");

    sql = CREATE_TRIPLE_INDEX_PO;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_PO created successfully. \n");

    sql = CREATE_TRIPLE_INDEX_OS;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "Index TRIPLE_OS created successfully. \n");

    // sql = CREATE_STRING_TABLE;
    // err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    // if( err != SQLITE_OK ){
    //     elog("SQL error: %s\n", zErrMsg);
    //     sqlite3_free(zErrMsg);
    //     return err;
    // }
    // ilog("%s", "STABLE created successfully\n");
    
    closeDB();
    return 0;
}


int SqliteBeanDB::reInit_()
{
    if (m_dir == nullptr || m_dir[0] == 0) return -1;

    int err = 0;
    err = closeDB();
    if (err != 0) return err;

    char buff[256] = {0};
    snprintf(buff, 256, "rm -rf %s/*.db", m_dir );
    //todo: check if command is executed successfully
    system(buff);
    err = internalInit();
    m_initialized = err == 0 ? true : false;
    return err;
}


int SqliteBeanDB::openDB()
{
    int err = 0;
    if (m_sqlite3Db_ == nullptr)  {
        err = sqlite3_open(m_dbFullPath.c_str(), &m_sqlite3Db_);
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


int SqliteBeanDB::createBean_(oidType &beanId)
{
    CHECK_CONNECTED();

    static const char sql[] = "INSERT INTO BEAN VALUES(?, ?, ?) ;";
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


int SqliteBeanDB::loadBeanBase_(oidType beanId, Json::Value& value, Json::Value* nativeData) 
{
    CHECK_CONNECTED();
    
    BeanWorld *world = getWorld();
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char SELECT_BEAN_1[] = "SELECT VALUE from BEAN WHERE ID = ?;";
    static const char SELECT_BEAN_2[] = "SELECT VALUE, NATIVE_DATA from BEAN WHERE ID = ?;";
    static const char SELECT_TRIPLE[] = "SELECT PID, OID from TRIPLE WHERE SID = ?;";
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
                *nativeData = Json::Value(Json::ValueType::objectValue);
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
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        } else {
            err = 0;
        }
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


//  std::list<std::string> SqliteBeanDB::getBeanProperties_(oidType id) const
// {
//     std::list<std::string> properties;
//     sqlite3_stmt *pstmt = nullptr;
//     Property* property = nullptr;
//     sqlite3_int64 value = 0;
//     const char* valueStr = nullptr;
//     static const char sql[] = "SELECT VALUE from BEAN WHERE ID = ?;";
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


int  SqliteBeanDB::loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value)
{
    CHECK_CONNECTED();

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
        err = loadBeanBase_(beanId, data);
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
        snprintf(buff, 64, "SELECT OID from TRIPLE WHERE SID = ? AND PID = %d;", 
            property->getId());
    } else {
        snprintf(buff, 64, "SELECT VALUE from p_%s WHERE SID = ?;", 
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
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}


int SqliteBeanDB::insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    CHECK_CONNECTED();

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
    bool alreadyInTransaction = false;

    SMART_BEGIN_TRANSACTION();
    if (err) {
        elog("Failed to insert bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isArray() && !property->isRelation()) {
        err = loadBeanBase_(beanId, data);
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
            err = saveBeanBase_(beanId, data);
            if (err) {
                elog("Failed to insert bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
                goto out;
            }
        }
    }

    if (property->isRelation()) {
        sql = "INSERT INTO TRIPLE  (ID, SID, PID, OID) VALUES(?, ?, ?, ?) ;";
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
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}


int SqliteBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    CHECK_CONNECTED();
    if (property == nullptr) return -1;
    if (property->isArray()) return -1;
    return updateBeanProperty_(beanId, property, (Json::ArrayIndex)-1, value);
}


int SqliteBeanDB::getIdByPropertyIndex(const Property* property, oidType sid, Json::ArrayIndex index, sqlite3_int64& id) const
{
    CHECK_CONNECTED();

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    Json::ArrayIndex i = 0;
    bool found = false;
    const char* pname = property->getName().c_str();
    pidType pid = property->getId();
    char buff[128]{0};
    char *sql = buff;

    if (property->isRelation()) {
        snprintf(buff, sizeof(buff), "SELECT ID FROM TRIPLE WHERE SID = ? AND PID = %d ;", property->getId());
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
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}


int SqliteBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) 
{
    CHECK_CONNECTED();

    char buff[128]{0};
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int nCol = 0;
    oidType sid = beanId;
    sqlite3_int64 id = 0;
    std::list<const Json::Value*> vlist;
    const Json::Value* v = nullptr;
    BeanWorld *world = nullptr;
   Json::Value data;
   const char* pname = nullptr;
   bool isArray = false;
   bool alreadyInTransaction = false;

    if (property == nullptr) return -1;

    world = getWorld();
    if (world == nullptr) return -1;

    if (!property->isArray() && (index != (Json::Value::ArrayIndex)-1)) return -1;

   pname = property->getName().c_str();
   isArray = property->isArray();

    if (isArray) {
        err = getIdByPropertyIndex(property, sid, index, id);
        if (err) {
            elog("Failed to upate property (name=%s) for bean (id=%llu), for index (%u) is invalid \n", pname, beanId, index);
            return err;
        }
    }

    SMART_BEGIN_TRANSACTION();
    if (err) {
        elog("Failed to update bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isDelayLoad() && !property->isRelation()) {
        err = loadBeanBase_(beanId, data);
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
            err = saveBeanBase_(beanId, data);
            if (err) {
                elog("Failed to update bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
                goto out;
            }
        }
    }

    v = &value;

    if (property->isRelation()) {
        if (isArray) {
            sql = "UPDATE TRIPLE SET OID = ? WHERE ID = ?  ;";
        } else {
            sql =  "UPDATE TRIPLE SET OID = ? WHERE SID = ?  ;";
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

    err = sqlite3_step(pstmt);

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}

int SqliteBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property) 
{
    CHECK_CONNECTED();

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
    bool alreadyInTransaction = false;

    if (!property->isRelation()) {
        err = loadBeanBase_(beanId, data);
        if (err) {
            elog("Failed to delete bean property (beanId=%llu, property name=%s) \n ", beanId, pname);
            goto out;
        }
    }

    SMART_BEGIN_TRANSACTION();
    if (err) {
        elog("Failed to delete bean property (beanId=%llu, property name=%s) \n", beanId, pname);
        return err;
    }

    if (!property->isRelation()) {
        if (data.isMember(pname)) {
            data.removeMember(pname);
            err = saveBeanBase_(beanId, data);
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
        sql = "DELETE FROM TRIPLE WHERE SID = ?;";
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
        if (err != SQLITE_OK) goto out;
        nCol = 1;
        err = sqlite3_bind_int64(pstmt, nCol++, sid);
        if (err != SQLITE_OK) goto out;
    } else {
        snprintf(buff, sizeof(buff), "DELETE FROM p_%s WHERE SID = ?;", pname);
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
        if (err != SQLITE_OK) goto out;
        nCol = 1;
        err = sqlite3_bind_int64(pstmt, nCol++, sid);
        if (err != SQLITE_OK) goto out;
    }

    err = sqlite3_step(pstmt);
    if (err != SQLITE_OK) goto out;

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}


int SqliteBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property, 
    Json::Value::ArrayIndex index) 
{
    CHECK_CONNECTED();

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
    bool alreadyInTransaction = false;

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

    SMART_BEGIN_TRANSACTION();
    if (err) {
        elog("Failed to delete bean property (beanId=%llu, property name=%s, index=%llu) \n", beanId, pname, index);
        return err;
    }

    snprintf(buff, sizeof(buff), "DELETE FROM p_%s WHERE ID = ?;", pname);
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    nCol = 1;
    err = sqlite3_bind_int64(pstmt, nCol++,  (sqlite3_int64)id);
    if (err != SQLITE_OK) goto out;

    err = sqlite3_step(pstmt);

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
}


int SqliteBeanDB::deleteBean_(oidType id)
{
    CHECK_CONNECTED();

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    bool alreadyInTransaction = false;
    static const char *sql = "DELETE FROM  BEAN WHERE ID = ?;";

    SMART_BEGIN_TRANSACTION();
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, (sqlite3_int64)id);
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );
    if (err != SQLITE_DONE) goto out;

    err = deleteRelationByObject(id);
    if (err != SQLITE_DONE) goto out;

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return err;
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


int SqliteBeanDB::loadProperties_(std::unordered_map<std::string, Property*>& properties) const
{
    CHECK_CONNECTED();

    static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE, INDEXED FROM PROPERTY;";
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
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


int SqliteBeanDB::undefineProperty_(Property* property)
{
    CHECK_CONNECTED();

    if (property == nullptr) return 0;

    int err = 0;
    const char* name = property->getName().c_str();
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* errMsg = nullptr;
    bool alreadyInTransaction = false;
    char buff[256]{0};
    int buffSize = sizeof(buff);
    static const char *sql_delete_property = "DELETE FROM  PROPERTY WHERE NAME = ?;";
    static const char *sql_delete_rtable = "DELETE FROM  TRIPLE WHERE PID = %d;";
    static const char drop_ptable[] =   "DROP TABLE p_%s; ";
    // static const char drop_index[] =   "DROP INDEX p_%s_index; ";

    SMART_BEGIN_TRANSACTION();
    if (err) {
        elog("Failed to undefine property (name =%s) \n",  name);
        return err;
    }

    err = deletePropertyFromAllBeans(property);
    if (err != 0) goto _out;

    if (property->isRelation()) {
        // snprintf(buff, buffSize, sql_delete_rtable, property->getId());
        // err = sqlite3_exec(m_sqlite3Db_, buff, nullptr , nullptr , &errMsg );
        // if (err != SQLITE_OK) goto _out;
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
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }

    if (errMsg  != nullptr) sqlite3_free(errMsg);
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


int SqliteBeanDB::defineProperty_(const char* name, 
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
    bool alreadyInTransaction = false;
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
    "CREATE INDEX p_%s_index \
    ON p_%s(VALUE); ";
    static const char sql_insert_ptable[] = "INSERT INTO PROPERTY VALUES(?, ?, ?, ?, ?) ;";

    bool isRelation = ((type == Property::RelationType || 
        type == Property::ArrayRelationType) ? true : false);
    bool isArray = ((type == Property::ArrayPrimaryType || 
        type == Property::ArrayRelationType) ? true : false);

    SMART_BEGIN_TRANSACTION();
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
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        if (errMsg != nullptr) elog("sqlite3 errormsg: %s \n", errMsg);
        SMART_ROLLBACK();
    }  else {
        err = 0;
        SMART_COMMIT();
        if (!err) {
            pid = (pidType)sqlite3_last_insert_rowid(m_sqlite3Db_);
        }
    }

    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    if (errMsg  != nullptr) sqlite3_free(errMsg);

    return err;
}


int SqliteBeanDB::beginTransaction_() 
{
    CHECK_CONNECTED();
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::commitTransaction_() 
{
    CHECK_CONNECTED();
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::rollbackTransaction_() 
{
    CHECK_CONNECTED();
    int err = 0;
    char* errMsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "ROLLBACK TRANSACTION", nullptr, nullptr, &errMsg);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errMsg);
    return err;
}


int SqliteBeanDB::saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value* nativeData)
{
    CHECK_CONNECTED();
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
    static const char *sql_1 = "UPDATE BEAN SET  VALUE = ? WHERE ID = ?;";
    static const char *sql_2 = "UPDATE BEAN SET  VALUE = ?, NATIVE_DATA = ? WHERE ID = ?;";

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

   if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err; 
}

int SqliteBeanDB::loadBeanNativeData_(oidType beanId, Json::Value& data)
{
    CHECK_CONNECTED();
    
    BeanWorld *world = nullptr;
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT NATIVE_DATA from BEAN WHERE ID = ?;";
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
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


int SqliteBeanDB::updateBeanNativeData_(oidType beanId, 
    const Json::Value& nativeData)
{
    CHECK_CONNECTED();

    int err = 0;
    char* errMsg = nullptr;
    Json::Reader reader;
    Json::FastWriter jsonWriter;
    Json::Value tmpValue;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    std::string valueStr;
    char* tmpStr = nullptr;
    static const char *sql = "UPDATE BEAN SET  NATIVE_DATA = ? WHERE ID = ?;";

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

   if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err; 
}

int SqliteBeanDB::deleteBeanNativeData_(oidType beanId)
{
    CHECK_CONNECTED();
    Json::Value value = Json::Value(Json::nullValue);
    return updateBeanNativeData_(beanId, value);
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
    static const char *sql = "DELETE FROM  TRIPLE WHERE OID = ?;";
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, id);
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}

int SqliteBeanDB::deletePropertyFromAllBeans(Property* property)
{
    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    bool alreadyInTransaction = false;
    const char* pname = property->getName().c_str();
    static const char SELECT_PTABLE[] = "SELECT SID from p_%s";
    static const char SELECT_TRIPLE[] = "SELECT SID from TRIPLE WHERE PID = %d;";
    char buff[128]{0};
    const char* sql = buff;
    const int COUNT = 128; //todo: what should be the best optimzed number?
    int countdown = 0;
    oidType sid = 0;
    std::list<oidType> sidList;
    bool finished = false;

    SMART_BEGIN_TRANSACTION();
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

        countdown = COUNT;
        sidList.clear();
        while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
            sid = sqlite3_column_int64(pstmt, 0);
            sidList.push_back(sid);
            countdown--;
            if (countdown == 0) break;
        }
        if (err != SQLITE_ROW) finished = true;

        sqlite3_clear_bindings(pstmt);
        sqlite3_reset(pstmt);
        sqlite3_finalize(pstmt);
        pstmt = nullptr;
        
        //delete bean property
        for (auto& item : sidList) {
            err = deleteBeanProperty_(item, property);
            if (err) goto out;
        }

    }

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        SMART_ROLLBACK();
    } else {
        err = 0;
        SMART_COMMIT();
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}

}
}
}