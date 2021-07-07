
#include "./SqliteBeanDB.h"
#include "./internal_common.hxx"

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
VALUE TEXT \
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
    int err = 0;
    err = openDB();
    if (err) {
        elog("Failed to connect to db. err=%d", err);
        return err;
    }
    err = loadProperties();
    if (err) {
        elog("Failed to load properties. err=%d", err);
        return err;
    }
    return 0;
}


int SqliteBeanDB::disconnect()
{
    if (!m_db) return 0;
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
    int err = 0;

    if (access(m_dbFullPath.c_str(), 0) == 0) return 0; //db file exists

    if (openDB()  !=  0) {
        elog("Failed to create database %s.\n", m_dbFullPath.c_str());
        return -1;
    } 

    char *zErrMsg = nullptr;
    const char *sql = nullptr;

    sql = CREATE_PTABLE;
    err = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    } 
    ilog("%s", "META_PTABLE created successfully\n");

    sql = CREATE_OTABLE;
    err = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "OTABLE created successfully\n");
    
    sql = CREATE_STABLE;
    err = sqlite3_exec(m_db, sql, nullptr, 0, &zErrMsg);
    if( err != SQLITE_OK ){
        elog("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return err;
    }
    ilog("%s", "STABLE created successfully\n");
    
    closeDB();
    return 0;
}


int SqliteBeanDB::reInit()
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
    if (m_db == nullptr)  {
        err = sqlite3_open(m_dbFullPath.c_str(), &m_db);
        if( err )  {
            elog("Failed to open database: %s\n", sqlite3_errmsg(m_db));
            m_db = nullptr;
        }
    }
    return err;
}

int SqliteBeanDB::closeDB()
{
    int err = 0;
    if (m_db == nullptr) return 0;
    err = sqlite3_close(m_db);
    if( err )  {
        elog("Failed to close database: %s\n", sqlite3_errmsg(m_db));
    } else {
        m_db = nullptr;
    }
    return err;
}


int SqliteBeanDB::loadAll()
{
    return 0;
}


int SqliteBeanDB::saveAll()
{
    return 0;
}


Bean* SqliteBeanDB::createBean()
{
    if (m_db == nullptr) return nullptr;

    BeanWorld *world = getWorld();
    if (world == nullptr) return nullptr;

    static const char sql[] = "INSERT INTO OTABLE VALUES(?, ?) ;";
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;

	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) return nullptr;

    err = sqlite3_bind_null(pstmt, 1);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_null(pstmt, 2);
    if (err != SQLITE_OK) goto out;
    
    err = sqlite3_step(pstmt);

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);

    sqlite3_int64 id = sqlite3_last_insert_rowid(m_db);

    return world->createBean(id);

}


Bean* SqliteBeanDB::getBean(oidType id)
{
    if (m_db == nullptr) return nullptr;
    
    BeanWorld *world = nullptr;
    if ((world = getWorld()) == nullptr) return nullptr;

    Bean* bean = world->getBean(id);
    if (bean != nullptr) return bean;

    const char* pname = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    // const char* pzTail = nullptr;
    int nCol = 0;
    int err = 0;
    int size = 0;
    bool notCreated = false;
    Property* property = nullptr;
    sqlite3_int64 value = 0;
    const char* valueStr = nullptr;
    static const char sql[] = "SELECT VALUE from OTABLE WHERE ID = ?;";
    Json::Reader reader; 
    Json::Value jsonBean;  

	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_int64(pstmt, 1, id);
    if (err != SQLITE_OK) goto _out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        bean = world->createBean((oidType)id);
        nCol = 0;
        if (sqlite3_column_type(pstmt, nCol++) != SQLITE_NULL) {
            valueStr = (const char*)sqlite3_column_text(pstmt, nCol++);
            if (!reader.parse(valueStr, jsonBean))
            {
                // err = -1;
                break;
            }

            for (auto& item : jsonBean.getMemberNames()) {
                pname = item.c_str();
                property = world->getProperty(pname);
                if (property != nullptr) { //it's a defined property
                    switch (property->getType()) {
                        case Property::PrimaryType:
                            bean->setProperty(property, jsonBean[pname]);
                            break;
                        case Property::RelationType:
                            bean->setRelation(property, jsonBean[pname].asUInt64());
                            break;
                        case Property::ArrayPrimaryType:
                            bean->createArrayProperty(property);
                            size = jsonBean[pname].size();
                            for (int i = 0; i < size; i++) {
                                bean->appendProperty(property,  jsonBean[pname][i]);
                            }
                            break;
                        case Property::ArrayRelationType:
                            bean->createArrayRelation(property);
                            size = jsonBean[pname].size();
                            for (int i = 0; i < size; i++) {
                                bean->appendRelation(property,  jsonBean[pname][i].asUInt64());
                            }            
                            break;
                        default: 
                            //shall not reach here
                            wlog("ignore invalid property of type: %d", property->getType());
                            break;
                    }
                } else { //unmanaged json value
                    bean->setUnmanagedValue(pname, jsonBean[pname]);
                }
            }
        }
	}
    if (err != SQLITE_DONE) {
        world->removeBean(id);
        bean = nullptr;
        goto _out;
    }

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return bean;
}


Bean* SqliteBeanDB::loadBean(oidType id)
{
    return 0;

}


int SqliteBeanDB::updateBean(Bean* bean)
{
    return 0;
}

int SqliteBeanDB::updateBean(Bean* bean, Property* property)
{
    if (bean == nullptr) return -1;
    if (property == nullptr) return -1;
    if (m_db == nullptr) return -1;
    return -1; //todo
}

int SqliteBeanDB::deleteBean(Bean* bean)
{
    if (bean == nullptr) return 0;
    if (m_db == nullptr) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    static const char *sql = "DELETE FROM  OTABLE WHERE ID = ?;";
    
	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_int64(pstmt, 1, (sqlite3_int64)bean->getId());
    if (err != SQLITE_OK) goto out;

	err = sqlite3_step( pstmt );
    if (err == SQLITE_DONE) {
        err = 0;
    }

out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }
    sqlite3_clear_bindings(pstmt);
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

int SqliteBeanDB::loadProperties()
{
    static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE, INDEXED FROM META_PTABLE;";
    BeanWorld *world = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int err = 0;
    int type = 0;
    int valueType = 0;
    const char *name = nullptr;
    // int64_t id = 0;

    if (m_db == nullptr) return -1;
    if ((world = getWorld()) == nullptr) return -2;

	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
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
    if (err == SQLITE_DONE) err = SQLITE_OK; 
 
 out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
}


int SqliteBeanDB::undefineProperty(const char* name)
{
    if (name == nullptr || name[0] == 0) return 0;

    Property *property = nullptr;
    BeanWorld *world = getWorld();
    if (world != nullptr) {
        property = world->getProperty(name);
        if (property == nullptr) return 0;
    }

    if (m_db == nullptr) return -1;

    int err = 0;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    char* errMsg = nullptr;
    char buff[128]{0};
    int buffSize = sizeof(buff);
    static const char *sql = "DELETE FROM  META_PTABLE WHERE NAME = ?;";
    static const char drop_table[] =   "DROP TABLE p_%s; ";

    sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) goto _out;
    err = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (err != SQLITE_OK) goto _out;
	err = sqlite3_step( pstmt );
    if (err != SQLITE_DONE) goto _out;
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);

    if (property->indexed() 
        || property->getValueType() == Property::StringType) {
        snprintf(buff, buffSize, drop_table, name);
        err = sqlite3_exec(m_db, buff, nullptr , nullptr , &errMsg );
        if (err != SQLITE_OK) goto _out;
    }

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        err = sqlite3_exec ( m_db , "ROLLBACK  TRANSACTION" , nullptr , nullptr , &errMsg ) ;
        elog("error occurred in %s, err=%d", __func__, err);
    } else {
        err = sqlite3_exec ( m_db , "COMMIT TRANSACTION" , nullptr , nullptr , &errMsg ) ;
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
        } else {
            if (world != nullptr) {
                world->undefineProperty(name);
            }
            err = 0;
        }
    }

    if (errMsg  != nullptr) sqlite3_free(errMsg);
    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    return err;
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
    if (name == nullptr || name[0] == 0) return nullptr;

    BeanWorld *world = getWorld();
    if (world == nullptr) return nullptr; //todo: currently world is required

    Property *property = nullptr;
    property = world->getProperty(name);
    if (property != nullptr) {
        if (property->getType() == type && property->getValueType() == valueType) {
            return property;
        } else {
            wlog("property with name %s already exists but with different type or value type.", name);
            return nullptr;
        }
    }

     //now let's create it in db
    if (m_db == nullptr) return nullptr;

    const char* sql = nullptr;
    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int err = 0;
    char* errMsg = nullptr;
    char  buff[128] {0};
    int sizeOfBuff = sizeof(buff);
    static const char create_property_table[] =   
    "CREATE TABLE p_%s ( \
    SUBJECT_ID BIGINT %s NOT NULL, \
    VALUE  %s NOT NULL \
    ); ";

    sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    static const char sql_insert_ptable[] = "INSERT INTO META_PTABLE VALUES(?, ?, ?, ?, ?) ;";
    sql = sql_insert_ptable;
	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt,nullptr);
    if (err != SQLITE_OK) return nullptr;

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

    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);

    if (needIndex
        || valueType ==  Property::StringType) {  
    //create a table for index or string reference
        snprintf(buff, sizeOfBuff - 1, "p_%s", name);
        
        snprintf(buff, sizeOfBuff - 1, create_property_table, 
        name,
        (type == Property::ArrayPrimaryType || type == Property::ArrayRelationType) ? "UNIQUE" : "",
        (valueType == Property::StringType) ? "TEXT" : "BIGINT" );
        sql = buff;
        err = sqlite3_exec ( m_db , sql , nullptr , nullptr , &errMsg ) ;
         if (err != SQLITE_OK)  goto _out;
    }

_out:
    if (err != SQLITE_OK && err != SQLITE_DONE) {
        err = sqlite3_exec ( m_db , "ROLLBACK  TRANSACTION" , nullptr , nullptr , &errMsg ) ;
        elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
    }  else {
        err = sqlite3_exec ( m_db , "COMMIT TRANSACTION" , nullptr , nullptr , &errMsg ) ;
        if (err != SQLITE_OK && err != SQLITE_DONE) {
            elog("sqlite3 errormsg: %s \n", sqlite3_errmsg(m_db));
        } else {
            switch (type) {
                case Property::PrimaryType:
                    property = world->defineProperty(name, (Property::ValueType)valueType, needIndex);
                    break;
                case Property::RelationType:
                    property = world->defineRelation(name, needIndex);
                    break;
                case Property::ArrayPrimaryType:
                    property = world->defineArrayProperty(name, (Property::ValueType)valueType, needIndex);
                    break;
                case Property::ArrayRelationType:
                    property = world->defineArrayRelation(name, needIndex);
                    break;
                default:
                    wlog("ignore invalid property of type: %d", type);
                    break;
            }
        }
    }

    sqlite3_clear_bindings(pstmt);
    sqlite3_reset(pstmt);
    sqlite3_finalize(pstmt);
    if (errMsg  != nullptr) sqlite3_free(errMsg);

    return property;
}


const Property* SqliteBeanDB::getProperty(const char* name) const
{
    return (const Property*) ((SqliteBeanDB*)this)->getProperty(name);
}


Property* SqliteBeanDB::getProperty(const char* name)
{
    if (name == nullptr || name[0] == 0) return nullptr;
    
    Property *property = nullptr;
    BeanWorld *world = getWorld();
    if (world != nullptr) {
        property = world->getProperty(name);
        if (property != nullptr) return property;
    } else {
        return nullptr; //todo: currently world is required
    }

    if (m_db == nullptr) return nullptr;

    sqlite3_stmt *pstmt = nullptr;
    const char* pzTail = nullptr;
    int nCol = 0;
    int err = 0;
    int type = 0;
    int valueType = 0;
    bool needIndex = false;
    // int64_t id = 0;
   static const char sql[] = "SELECT ID, NAME, PTYPE, VTYPE, INDEXED FROM META_PTABLE WHERE NAME = ?;";

	err = sqlite3_prepare_v2(m_db, sql, strlen(sql), &pstmt, nullptr);
    if (err != SQLITE_OK) goto out;
    err = sqlite3_bind_text(pstmt, 1, name, -1, nullptr);
    if (err != SQLITE_OK) goto out;

	while((err = sqlite3_step( pstmt )) == SQLITE_ROW) {
        nCol = 1;
        name = (const char*)sqlite3_column_text(pstmt, nCol++);
        type = sqlite3_column_int(pstmt, nCol++);
        valueType = sqlite3_column_int(pstmt, nCol++);
        needIndex = sqlite3_column_int(pstmt, nCol++) == 1 ? true : false;
        switch (type) {
            case Property::PrimaryType:
                property = world->defineProperty(name, (Property::ValueType)valueType, needIndex);
                break;
            case Property::RelationType:
                property = world->defineRelation(name, needIndex);
                break;
            case Property::ArrayPrimaryType:
                property = world->defineArrayProperty(name, (Property::ValueType)valueType, needIndex);
                break;
            case Property::ArrayRelationType:
                property = world->defineArrayRelation(name, needIndex);
                break;
            default:
                wlog("ignore invalid property of type: %d", type);
                break;
        }
        break;
	}
    if (err != SQLITE_ROW) {
        elog("error occurred in %s, err=%d", __func__, err);
    } 
 
out:
    sqlite3_reset(pstmt);
	sqlite3_finalize(pstmt);

    return property;
}


}
}
}