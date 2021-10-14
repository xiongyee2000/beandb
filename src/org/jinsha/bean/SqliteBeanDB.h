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


private:
    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int reInit_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int connect_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int disconnect_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int beginTransaction_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int commitTransaction_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int rollbackTransaction_() override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int defineProperty_(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int undefineProperty_(Property* property) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int loadProperties_(std::unordered_map<std::string, Property*>& properties) const override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int createBean_(oidType &id);

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int loadBeanBase_(oidType beanId, Json::Value& value, Json::Value* nativeData = nullptr) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value* nativeData = nullptr) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int deleteBean_(oidType id) override;

    // /**
    //  * @ref BeanDBPersistenceIntf
    //  */
    // virtual std::list<std::string> getBeanProperties_(oidType id) const override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;
    
    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int loadBeanNativeData_(oidType beanId, Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int updateBeanNativeData_(oidType beanId, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual int deleteBeanNativeData_(oidType beanId) override;

    /**
     * @ref BeanDBPersistenceIntf
     */
   BeanIdPage* findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref BeanDBPersistenceIntf
     */
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    
private:
    class SqliteBeanIdPage : public BeanIdPage {
        public:
        SqliteBeanIdPage(unsigned int pageSize, BeanIdPage::LoadPageFuncType func) : BeanIdPage(pageSize, func) {};
    };

    int loadPage_findSubjects(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize, unsigned long pageIndex, std::vector<oidType>& sids);

private:
    int initDB();
    int checkDB();
    int openDB();
    int closeDB();
    static bool determineDelayLoad(Property::Type type, Property::ValueType valueType);
    int getIdByPropertyIndex(const Property* property, oidType sid, Json::ArrayIndex index, sqlite3_int64& id) const;
    int deleteRelationByObject(oidType id);    
    int deletePropertyFromAllBeans(Property* property);

private:
    std::string m_dir_;
    std::string m_dbFullPath_;
    sqlite3* m_sqlite3Db_;
    bool m_initialized_;
    bool m_deletePropertyFromBeans_;

};

}
}
}