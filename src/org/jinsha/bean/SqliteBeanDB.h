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
     * @ref BeanDBPIntf
     */
    virtual int reInit() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int connect_() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int disconnect_() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int beginTransaction_() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int commitTransaction_() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int rollbackTransaction_() override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int defineProperty_(const char* name, 
        Property::Type type,
        Property::ValueType valueType, 
        pidType& pid,
        bool& delayLoad) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int undefineProperty_(Property* property) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int loadProperties_(std::unordered_map<std::string, Property*>& properties) const override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int createBean_(oidType &id);

    /**
     * @ref BeanDBPIntf
     */
    virtual int loadBeanBase_(oidType beanId, Json::Value& value, Json::Value* nativeData = nullptr) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int saveBeanBase_(oidType beanId, const Json::Value& data, const Json::Value* nativeData = nullptr) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int deleteBean_(oidType id) override;

    // /**
    //  * @ref BeanDBPIntf
    //  */
    // virtual std::list<std::string> getBeanProperties_(oidType id) const override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int loadBeanProperty_(oidType beanId, const Property* property, Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int insertBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int updateBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex  index,
        const Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int deleteBeanProperty_(oidType beanId, 
        const Property* property, 
        Json::Value::ArrayIndex index) override;
    
    /**
     * @ref BeanDBPIntf
     */
    virtual int loadBeanNativeData_(oidType beanId, Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int updateBeanNativeData_(oidType beanId, 
        const Json::Value& value) override;

    /**
     * @ref BeanDBPIntf
     */
    virtual int deleteBeanNativeData_(oidType beanId) override;

    /**
     * @ref BeanDBPIntf
     * 
     * Notes:
     * - When optype is op_like, the property and value shall be of StringType,
     *    and the value shall contain a string with compliance to sqlite3 like clause.
     */
   BeanIdPage* findBeans(opType optype, const Property* property, const Json::Value& value, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref BeanDBPIntf
     */
    virtual BeanIdPage* findSubjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref BeanDBPIntf
     */
    virtual BeanIdPage* findObjects(const Property* property, unsigned int pageSize = DEFAULT_PAGE_SIZE) const override;

    /**
     * @ref BeanDBPIntf
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