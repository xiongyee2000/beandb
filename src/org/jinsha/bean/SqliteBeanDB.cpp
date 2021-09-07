
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"
#include "./Bean.h"

#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include "jsoncpp/json/value.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"

namespace org {
namespace jinsha {
namespace bean {

const char* SqliteBeanDB::DB_PATH  =  "beandb.db";
// const char* SqliteBeanDB::PTABLE_NAME = "PTABLE";
// const char* SqliteBeanDB::OTABLE_NAME = "OTABLE";

static const char* CREATE_PTABLE =  
 "CREATE TABLE META_PTABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
NAME VARCHAR(64) UNIQUE NOT NULL, \
PTYPE INT NOT NULL, \
VTYPE INT NOT NULL, \
INDEXED INT8 NOT NULL \
); ";

static const char* CREATE_OTABLE =  
 "CREATE TABLE OTABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
VALUE TEXT DEFALT NULL,  \
UNMANAGED_VALUE DEFALT NULL \
); ";

static const char* CREATE_STABLE =  
 "CREATE TABLE STABLE ( \
ID INTEGER PRIMARY KEY NOT NULL, \
VALUE TEXT NOT NULL \
); ";

static const char* CREATE_NDTABLE =  
 "CREATE TABLE NATIVE_DATA ( \
ID INTEGER PRIMARY KEY NOT NULL, \
SID BIGINT NOT NULL, \
VALUE TEXT NOT NULL \
); ";

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
        return err;
    }
    return 0;
}


int SqliteBeanDB::disconnect_()
{
    if (!m_sqlite3Db_) return 0;
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

    sql = CREATE_PTABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "META_PTABLE created successfully\n");

    sql = CREATE_OTABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "OTABLE created successfully\n");
    
    sql = CREATE_STABLE;
    err = sqlite3_exec(m_sqlite3Db_, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "STABLE created successfully\n");
    
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
    snprintf(buff, 255, "rm -rf %s/*.db", m_dir );
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
    if (m_sqlite3Db_ == nullptr) return -1;

    static const char sql[] = "INSERT INTO OTABLE VALUES(?, ?, ?) ;";
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


int SqliteBeanDB::loadBeanBase_(oidType beanId, Json::Value& value, Json::Value& nativeData) 
{
    if (m_sqlite3Db_ == nullptr) return -1;
    
    BeanWorld *world = nullptr;
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT VALUE, UNMANAGED_VALUE from OTABLE WHERE ID = ?;";
    Json::Reader reader; 
    Json::Value jsonBean;  
    bool found = false;
    Json::Value* v = nullptr;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, 1, beanId);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        //retrieve properties
        if (sqlite3_column_type(pstmt, 0) == SQLITE_NULL) continue;

        valueStr = (const char*)sqlite3_column_text(pstmt, 0);
        if (valueStr == nullptr) valueStr = "{}";
        if (!reader.parse(valueStr, value, false))
        {
            err = -1;
            elog("error parsing json string: %s", valueStr);
            break;
        }

        for (auto& pname : value.getMemberNames()) {
            property = getWorld()->getProperty(pname.c_str());
            if (property == nullptr) continue; //it's not a defined property
            //filter out delay load properties
            if (isDelayLoad(*property)) {
                value[pname] = Json::Value::nullRef;
            } else {
                if (property->getType() == Property::ArrayPrimaryType || 
                     property->getType() == Property::ArrayRelationType) {
                         for (Json::ArrayIndex i = 0; i < value.size(); i++) {
                             v = &value[pname][i];
                            switch (property->getValueType())
                            {
                            case Property::IntType:
                                *v= (*v).asInt64();
                                break;
                            case Property::UIntType:
                                *v= (*v).asUInt64();
                                 break;
                            case Property::RealType:
                                *v= (*v).asDouble();
                                 break;
                            case Property::BoolType:
                                break;
                            //do nothing
                            case Property::StringType:
                                //do nothing
                                break;
                            default:
                                break;
                            }
                         }
                } else {
                    Json::Value* v = &value[pname];
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
                    case Property::StringType:
                        //do nothing
                        break;
                    default:
                        break;
                    }
                }

            }
        }

        //retrieve native data
        if (sqlite3_column_type(pstmt, 1) != SQLITE_NULL) {
            valueStr = (const char*)sqlite3_column_text(pstmt, 1);
            if (valueStr == nullptr) valueStr = "{}";
            if (!reader.parse(valueStr, nativeData, false))
            {
                err = -2;
                elog("error parsing json string: %s", valueStr);
                break;
            }
        } else {
            nativeData = Json::Value(Json::ValueType::objectValue);
        }
        
        found = true;
        err = 0;
        break;
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


 std::list<std::string> SqliteBeanDB::getBeanProperties_(oidType id) const
{
    std::list<std::string> properties;
    sqlite3_stmt *pstmt = nullptr;
    Property* property = nullptr;
    sqlite3_int64 value = 0;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT VALUE from OTABLE WHERE ID = ?;";
    Json::Reader reader; 
    Json::Value jsonBean;
    int err = 0;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, 1, id);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        if (sqlite3_column_type(pstmt, 0) != SQLITE_NULL) {
            valueStr = (const char*)sqlite3_column_text(pstmt, 0);
            if (!reader.parse(valueStr, jsonBean))
                break;
            for (auto& item : jsonBean.getMemberNames()) {
                properties.push_back(item);
            }
        }
    }

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return properties;
}


int  SqliteBeanDB::loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value)
{
    if (m_sqlite3Db_ == nullptr) return -1;
    // bool connected = this->connected();
    // if (!connected) {
    //     if (0 != connect()) {
    //         return -1;
    //     }
    // }

    int err = 0;
    oidType id = 0; //id of the string in property table
    const char* pname = property->getName().c_str();
    char buff[128]{0};
    char * sql = &buff[0];
    sqlite3_stmt *pstmt = nullptr;
    Json::Value* v = nullptr;
    int i = 0;

    //set to null first
    value = Json::Value::nullRef;

    if (isDelayLoad(*property)) {
        snprintf(buff, 64, "SELECT VALUE from p_%s WHERE SID = ?;", 
            pname);
        err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int64(pstmt, 1, beanId);
        if (err != SQLITE_OK) goto out;

        while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
            //do some preparation for array
            if (property->getType() == Property::ArrayPrimaryType ||
                 property->getType() == Property::ArrayRelationType)  {
                    if (value.isNull())  
                        value = Json::Value(Json::arrayValue);
                    value.append(Json::Value::nullRef);
                    v = &value[value.size() - 1];
            } else {
                v = &value;
            }

            //set value according to value type
            switch (property->getValueType())
            {
            case Property::IntType:
                (*v) = sqlite3_column_int64(pstmt, 0);
                break;
            case Property::UIntType:
                (*v) = (Json::UInt64)sqlite3_column_int64(pstmt, 0);
                break;
            case Property::RealType:
                (*v) = sqlite3_column_double(pstmt, 0);
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
    } else  {
        Json::Value data;
        Json::Value nativeData;
        err = loadBeanBase_(beanId, data, nativeData);
        if (err)  goto out;
        if (data.isMember(pname))
            value = data[pname];
    }

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        value = Json::Value::nullRef;
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    // if (!connected) { 
    //     //if previously not connected, resume it to disconnected
    //     disconnect();
    // }
    return err;

}


int SqliteBeanDB::insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    if (property == nullptr) return -1;
    if (m_sqlite3Db_ == nullptr) return -1;
     //no need to do anything here for non-indexed
     //and non-string-valued property
    if (!property->indexed() && 
        property->getValueType() != Property::StringType) 
    return 0; 

    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    const char* pname = property->getName().c_str();
    char buff[128]{0};
    snprintf(buff, sizeof(buff) - 1, "INSERT INTO p_%s  (ID, SID, VALUE) VALUES(?, ?, ?) ;", pname);
    const char* sql = buff;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;

    oidType sid = beanId;
    std::list<const Json::Value*> vlist;
    
    if (value.isArray()) {
        for (int i = 0; i < value.size(); i++) {
            vlist.push_back(&value[i]);
        }
    } else {
        vlist.push_back(&value);
    }

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    for (const auto& v : vlist) {
        nCol = 1;
        err = sqlite3_bind_null(pstmt, nCol++);
        if (err != SQLITE_OK) goto out;
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)sid);
        if (err != SQLITE_OK) goto out;
        if (property->getValueType() == Property::StringType) {
            err = sqlite3_bind_text(pstmt, nCol++, v->asCString(), -1, nullptr);
        } else {
            err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)v->asInt64());
        }
        if (err != SQLITE_OK) goto out;
        
        err = sqlite3_step(pstmt);
         if (err != SQLITE_OK) goto out;
    }

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


int SqliteBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) 
{
    return updateBeanProperty_(beanId, property, (Json::ArrayIndex)-1, value);
}


int SqliteBeanDB::getIdByPropertyIndex(const char* pname, Json::ArrayIndex index, sqlite3_int64& id)
{
    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    Json::ArrayIndex i = 0;
    bool found = false;
    static const char sql[] = "SELECT ID FROM p_? WHERE SID = ? ;";
    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, id);
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

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    return found ? 0 : -1;
}


int SqliteBeanDB::updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) 
{
    if (property == nullptr) return -1;
    if (m_sqlite3Db_ == nullptr) return -1;
     //no need to do anything here for non-indexed
     //and non-string-valued property
    if (!property->indexed() && 
        property->getValueType() != Property::StringType) 
    return 0; 

    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    bool isArray = false;
    if (property->getType() == Property::ArrayPrimaryType ||
        property->getType() == Property::ArrayRelationType) 
        isArray = true;

    if (!isArray && (index != (Json::Value::ArrayIndex)-1)) return -1;

    static const char sql[] = "UPDATE p_? SET VALUE = ? WHERE ? = ? ;";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;
    const char* pname = property->getName().c_str();
    oidType sid = beanId;
    sqlite3_int64 id = 0;
    std::list<const Json::Value*> vlist;
    const Json::Value* v = nullptr;
    
    if (isArray) {
        err = getIdByPropertyIndex(pname, index, id);
        if (err) return -12;
        v = &value[index];
    }  else {
        v = &value;
    }

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) return -2;
    nCol = 0;
    err = sqlite3_bind_text(pstmt, nCol++, pname, -1, nullptr);
    if (err != SQLITE_OK) goto out;
    if (property->getValueType() == Property::StringType) {
        err = sqlite3_bind_text(pstmt, nCol++, v->asCString(), -1, nullptr);
    } else {
        err = sqlite3_bind_int64(pstmt, nCol++, (sqlite3_int64)v->asInt64());
    }
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_text(pstmt, nCol++, isArray ? "ID" : "SID", -1, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, nCol++, isArray ? (sqlite3_int64)id : (sqlite3_int64)sid);
    if (err != SQLITE_OK) goto out;

    err = sqlite3_step(pstmt);
        if (err != SQLITE_OK) goto out;

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

int SqliteBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property) 
{
    return deleteBeanProperty_(beanId, property, (Json::Value::ArrayIndex)-1);
}

int SqliteBeanDB::deleteBeanProperty_(oidType beanId, 
    const Property* property, 
    Json::Value::ArrayIndex index) 
{
    if (property == nullptr) return -1;
    if (m_sqlite3Db_ == nullptr) return -1;

    //todo: remove propoerty value from bean record

    //  //no need to do anything here for non-indexed
    //  //and non-string-valued property
    // if (!property->indexed() && 
    //     property->getValueType() != Property::StringType) 
    // return 0; 


    BeanWorld *world = getWorld();
    if (world == nullptr) return -1;

    bool isArray = false;
    if (property->getType() == Property::ArrayPrimaryType ||
        property->getType() == Property::ArrayRelationType) 
        isArray = true;

    if (!isArray && (index != (Json::Value::ArrayIndex)-1)) return -1;

    static const char sql[] = "DELETE FROM p_? WHERE ? = ? ;";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    int nCol = 0;
    const char* pname = property->getName().c_str();
    oidType sid = beanId;
    sqlite3_int64 id = 0;
    std::list<const Json::Value*> vlist;
    const Json::Value* v = nullptr;
    
    if (isArray) {
        err = getIdByPropertyIndex(pname, index, id);
        if (err) return -12;
    } 

    err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) return -2;
    nCol = 0;
    err = sqlite3_bind_text(pstmt, nCol++, pname, -1, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_text(pstmt, nCol++, isArray ? "ID" : "SID", -1, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, nCol++, isArray ? (sqlite3_int64)id : (sqlite3_int64)sid);
    if (err != SQLITE_OK) goto out;

    err = sqlite3_step(pstmt);
        if (err != SQLITE_OK) goto out;
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);

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


int SqliteBeanDB::deleteBean_(Bean* bean)
{
    if (bean == nullptr) return 0;
    if (m_sqlite3Db_ == nullptr) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    static const char *sql = "DELETE FROM  OTABLE WHERE ID = ?;";
    
	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, (sqlite3_int64)bean->getId());
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );
    if (err == SQLITE_DONE) {
        err = 0;
    }

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    ////
    //todo
    ////
    // bean->removeAllProperties();
    // //handle relations: remove relation from subject that has
    // //relation to this bean (as object)

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
    static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE, INDEXED FROM META_PTABLE;";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int err = 0;
    int type = 0;
    int valueType = 0;
    bool indexed = false;
    const char *name = nullptr;
    int id = 0;

    if (m_sqlite3Db_ == nullptr) return -1;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        nCol = 0;
        id = sqlite3_column_int(pstmt, nCol++);
        name = (const char*)sqlite3_column_text(pstmt, nCol++);
        type = sqlite3_column_int(pstmt, nCol++);
        valueType = sqlite3_column_int(pstmt, nCol++);
        indexed = (sqlite3_column_int(pstmt, nCol++) == 1 ? true : false);
        Property* property = newProperty(name,  
            (pidType)id, 
            (Property::Type)type, 
            (Property::ValueType)valueType, 
            indexed);
        if (valueType == Property::StringType) 
            setDelayLoad(*property, true);
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


int SqliteBeanDB::undefineProperty_(const char* name)
{
    if (name == nullptr || name[0] == 0) return 0;
    if (m_sqlite3Db_ == nullptr) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* errMsg = nullptr;
    char buff[256]{0};
    int buffSize = sizeof(buff);
    static const char *sql = "DELETE FROM  META_PTABLE WHERE NAME = ?;";
    static const char drop_table[] =   "DROP TABLE p_%s; ";

    err = beginTransaction();
    if (err) return err;

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (err != SQLITE_OK) goto _out;
	err = sqlite3_step( pstmt );
    if (err != SQLITE_DONE) goto _out;
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);

    snprintf(buff, buffSize, drop_table, name);
    err = sqlite3_exec(m_sqlite3Db_, buff, nullptr , nullptr , &errMsg );
    if (err != SQLITE_OK) goto _out;

    //todo: remove property value from beans

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
        err = rollbackTransaction();
    } else {
        err = commitTransaction();
    }

    if (errMsg  != nullptr) sqlite3_free(errMsg);
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


pidType SqliteBeanDB::defineProperty_(const char* name, 
    Property::Type type, 
    Property::ValueType valueType, 
    bool needIndex)
{
    if (name == nullptr || name[0] == 0) return -1;

     //now let's create it in db
    if (m_sqlite3Db_ == nullptr) return -2;

    const char* sql = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    pidType pid = -1;
    char* errMsg = nullptr;
    char  buff[256] {0};
    int sizeOfBuff = sizeof(buff);
    static const char create_property_table[] =   
    "CREATE TABLE p_%s ( \
    ID INTEGER PRIMARY KEY NOT NULL, \
    SID BIGINT %s NOT NULL, \
    VALUE  %s NOT NULL \
    ); ";

    err = beginTransaction();
    if (err) return err;

    static const char sql_insert_ptable[] = "INSERT INTO META_PTABLE VALUES(?, ?, ?, ?, ?) ;";
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
    err = sqlite3_bind_int(pstmt, 5, needIndex ? 1 : 0);
    if (err != SQLITE_OK) goto _out;
    
    err = sqlite3_step(pstmt);
    if (err != SQLITE_DONE) goto _out;

    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);

    //create property table
    snprintf(buff, sizeOfBuff - 1, create_property_table, 
    name,
    (type == Property::ArrayPrimaryType || type == Property::ArrayRelationType) ? "UNIQUE" : "",
    (valueType == Property::StringType) ? "TEXT" : "BIGINT" );
    sql = buff;
    err = sqlite3_exec ( m_sqlite3Db_ , sql , nullptr , nullptr , &errMsg ) ;
        if (err != SQLITE_OK)  goto _out;

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        err = rollbackTransaction();
    }  else {
        err = commitTransaction();
        if (!err) {
            pid = (pidType)sqlite3_last_insert_rowid(m_sqlite3Db_);
        }
    }

    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    if (errMsg  != nullptr) sqlite3_free(errMsg);

    return pid;
}


int SqliteBeanDB::beginTransaction_() 
{
    if (m_sqlite3Db_ == nullptr) return -1;
    int err = 0;
    char* errmsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errmsg);
    return err;
}


int SqliteBeanDB::commitTransaction_() 
{
    if (m_sqlite3Db_ == nullptr) return -1;
    int err = 0;
    char* errmsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errmsg);
    return err;
}


int SqliteBeanDB::rollbackTransaction_() 
{
    if (m_sqlite3Db_ == nullptr) return -1;
    int err = 0;
    char* errmsg = nullptr;
    err = sqlite3_exec(m_sqlite3Db_, "ROLLBACK TRANSACTION", nullptr, nullptr, &errmsg);
    if (err != SQLITE_OK) 
        elog("sqlite3 errormsg: %s \n", errmsg);
    return err;
}


int SqliteBeanDB::saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value& nativeData)
{
    if (m_sqlite3Db_ == nullptr) return -1;
    if (data.isNull()) return -2;

    int err = 0;
    sqlite3_int64 beanId_ = (sqlite3_int64)beanId;
    char* errMsg = nullptr;
    Json::Reader reader;
    Json::FastWriter jsonWriter;
    Json::Value tmpValue;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* tmpStr = nullptr;
     std::string valueStr;
     const Property* property = nullptr;
    static const char *sql = "UPDATE OTABLE SET  VALUE = ?, UNMANAGED_VALUE = ? WHERE ID = ?;";

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;

    for (auto& pname : data.getMemberNames()) {
        property = getWorld()->getProperty(pname.c_str());
        if (property == nullptr) continue; //todo: error here?
        if (!isDelayLoad(*property)) {
            tmpValue[pname] = data[pname];
        } else {
            tmpValue[pname] = Json::Value::nullRef;
        }
    }

    valueStr = jsonWriter.write(tmpValue);
    tmpStr = sqlite3_mprintf("%q", valueStr.c_str());
    err = sqlite3_bind_text(pstmt, 1, tmpStr , -1, nullptr);
    if (err != SQLITE_OK) goto _out;

    if (nativeData.isNull()) {
        err = sqlite3_bind_null(pstmt, 2);
    }  else {
        std::string nativeDataStr = jsonWriter.write(nativeData);
        tmpStr = sqlite3_mprintf("%q", nativeDataStr.c_str());
        err = sqlite3_bind_text(pstmt, 2, tmpStr , -1, nullptr);
    }
    if (err != SQLITE_OK) goto _out;

    err = sqlite3_bind_int64(pstmt, 3, beanId_);
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
   if (m_sqlite3Db_ == nullptr) return -1;
    
    BeanWorld *world = nullptr;
    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    int err = 0;
    int size = 0;
    Property* property = nullptr;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT UNMANAGED_VALUE from OTABLE WHERE ID = ?;";
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
            valueStr = (const char*)sqlite3_column_text(pstmt, 1);
            if (valueStr == nullptr) valueStr = "{}";
            if (!reader.parse(valueStr, data, false))
            {
                err = -2;
                elog("error parsing json string: %s \n", valueStr);
                break;
            }
        } else {
            data = Json::Value(Json::ValueType::objectValue);
        }
        
        found = true;
        err = 0;
        break;
	}

_out:
    if (err > 0 && err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_sqlite3Db_));
    } else {
        err = 0;
    }
    if (pstmt != nullptr) sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}

int SqliteBeanDB::insertBeanNativeData_(oidType beanId, 
    const Json::Value& value)
{
    int err = 0;
    //todo:
    return err;
}

int SqliteBeanDB::updateBeanNativeData_(oidType beanId, 
    const Json::Value& nativeData)
{
   if (m_sqlite3Db_ == nullptr) return -1;

    int err = 0;
    char* errMsg = nullptr;
    Json::Reader reader;
    Json::FastWriter jsonWriter;
    Json::Value tmpValue;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    std::string valueStr;
    char* tmpStr = nullptr;
    static const char *sql = "UPDATE OTABLE SET  UNMANAGED_VALUE = ? WHERE ID = ?;";

	err = sqlite3_prepare_v2(m_sqlite3Db_, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;

    if (nativeData.isNull()) {
        tmpStr = "{}";
    }  else {
        valueStr = jsonWriter.write(nativeData);
        tmpStr = sqlite3_mprintf("%q", valueStr.c_str());
    }
    err = sqlite3_bind_text(pstmt, 1, tmpStr , -1, nullptr);
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

int SqliteBeanDB::deleteBeanNativeData_(oidType beanId, 
    const Json::Value& value)
{
    int err = 0;
    //todo:
    return err;
}

}
}
}