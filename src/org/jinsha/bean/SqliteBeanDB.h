#pragma once

#include "./AbstractBeanDB.h"

#include <sqlite3.h>
#include <unordered_map>

#include "./Page.hpp"
#include "./internal_common.hxx"

namespace org {
namespace jinsha {
namespace bean {

class SqliteBeanDB : public AbstractBeanDB
{
public:
    /**
     * Constructor
     * 
     * @param dir the directory where data are stored
     */
    SqliteBeanDB(const char* dir);

    /**
     * Destructor
     */
    virtual ~SqliteBeanDB();

    /**
     * Get the directory where data are stored.
     * 
     * @return the directory
     */
    const std::string& getDir() const  {return m_dir_;};

public:
    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - This method will directly call reInit();
     */
    virtual int clear() override;

    /**
     * Re-initialize the database.
     * 
     * CAUTION: 
     * All data in the storage will be unrecoverably deleted!
     * 
     * Notes:
     * - This method is used to re-initialize the database, 
     *    e.g. recreate data structure etc.
     * - This method must be called while the database
     *    is disconnected.
     * 
     * @return 0 for success, or an error code
     */
    virtual int reInit();

private:
    /***********************************************************
     * transaction related
     ***********************************************************/
    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int beginTransaction();

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int commitTransaction();

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    virtual int rollbackTransaction();

    /**
     * Check if it is in a transaction
     * 
     * @return true if in a transaction, or false if not
     */
    virtual bool inTransaction();

    /**
     * Begin a transaction.
     * 
     * @return 0 on success, or an error code
     */
    int doBeginTransaction();

    /**
     * Commit a transaction.
     * 
     * @return 0 on success, or an error code
     */
    int doCommitTransaction();

    /**
     * Rollback a transaction.
     * 
     * @return 0 on success, or an error code
     */
    int doRollbackTransaction();

    /**
     * @ref AbstractBeanDB
     */
    virtual int doConnect() override;
    
    /**
     * @ref AbstractBeanDB
     */
    virtual int doDisconnect() override;

//  std::list<std::string> getBeanProperties_(oidType id) const;

    /**
     * @ref AbstractBeanDB
     */
    virtual int defineProperty(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int undefineProperty(Property* property) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int loadProperties(std::unordered_map<std::string, Property*>& properties) const override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int createBean(oidType &id);

    /**
     * @ref AbstractBeanDB
     */
    virtual int loadBeanBase(oidType beanId, Json::Value& value, Json::Value* nativeData = nullptr) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int saveBeanBase(oidType beanId, const Json::Value& data, const Json::Value* nativeData = nullptr) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int deleteBean(oidType id) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int loadBeanProperty(oidType beanId, const Property* property, Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int insertBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - 0 will be returned if no such bean exist (take it as success)
     */
    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - 0 will be returned if no such bean exist (take it as success)
     */
    virtual int updateBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - 0 will be returned if no such bean exist (take it as success)
     * 
     */
    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property) override;

    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - 0 will be returned if no such bean exist (take it as success)
     * 
     */
    virtual int deleteBeanProperty(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;
    
    /**
     * @ref AbstractBeanDB
     */
    virtual int loadBeanNativeData(oidType beanId, Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int updateBeanNativeData(oidType beanId, 
        const Json::Value& value) override;

    /**
     * @ref AbstractBeanDB
     */
    virtual int deleteBeanNativeData(oidType beanId) override;

    /**
     * @ref AbstractBeanDB
     * 
     * Notes:
     * - When optype is op_like, the property and value shall be of StringType,
     *    and the value shall contain a string with compliance to sqlite3 like clause.
     */
   BeanIdPage* findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref AbstractBeanDB
     */
    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref AbstractBeanDB
     */
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref AbstractBeanDB
     */
    virtual BeanIdPage* getAllBeans(unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    
private:
    class SqliteBeanIdPage : public BeanIdPage {
        public:
        SqliteBeanIdPage(unsigned int pageSize, BeanIdPage::LoadPageFuncType func) : BeanIdPage(pageSize, func) {};

        public:
        Json::Value currPageMinValue;
        Json::Value currPageMaxValue;
        Json::Value minValue;
        Json::Value maxValue;
    };

    int loadPage_findBeans(opType optype, const Property* property, const Json::Value& value, BeanIdPage* page,  unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids);
    int loadPage_findSubjects_Objects(bool findSubjects, const Property* property, BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids);
    int loadPage_getAllBeans(BeanIdPage* page, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids);

private:
    int initDB();
    int checkDB();
    int openDB();
    int closeDB();
    static bool determineDelayLoad(Property::Type type, Property::ValueType valueType);
    int getIdByPropertyIndex(const Property* property, oidType sid, Json::ArrayIndex index, sqlite3_int64& id) const;
    int deleteRelationByObject(oidType id);    
    int deletePropertyFromAllBeans(Property* property);
    BeanIdPage* findSubjectsObjects(bool findSubjects, const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const;
    int handleErrAtEnd(int err, bool finishTransaction);
    int checkConnected(const char* errMsg) const;

private:
    std::string m_dir_;
    std::string m_dbFullPath_;
    sqlite3* m_sqlite3Db_;
    bool m_initialized_;
    int m_transactionCount_;
    bool m_deletePropertyFromBeans_;

};

}
}
}