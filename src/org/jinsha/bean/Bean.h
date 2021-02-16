#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "./common.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanWorld;

class Bean
{
public:
    // otype getClassId() {return classId_;};
    // std::string getClassName() {return className_;};
    // std::string getClassUri() {return classUri_;};

    /**
     * Get bean id.
     * 
     * @return the bean id
     */
    inline oidType getId() {return m_id_;};
    // std::string &getName() {return m_name_;};
    // std::string &getUri() {return uri_;};

    // void setClassId(otype id) {classId_ = id;};
    // void setClassName(const std::string& name) {className_ = name;};
    // void setClassUri(const std::string& uri) {classUri_ = uri;};

    // void setId(otype id) {m_id_ = id;};
    // void setName(const std::string& name) {m_name_ = name;};
    // void setUri(const std::string& uri) {uri_ = uri;};

    ////////////////////////////////////////////////////////////////
    //Below  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////
    bool 	empty () const {return m_jsonValue_.empty();};

    Json::Value get (const char *key, const Json::Value &defaultValue) const 
        {return m_jsonValue_.get(key, defaultValue);};

    Json::Value get (const std::string &key, const Json::Value &defaultValue) const 
        {return m_jsonValue_.get(key, defaultValue);};

    bool isMember (const char* key) const;
    bool isMember (const std::string &key) const;

    Json::Value::Members getMemberNames () const 
        {return m_jsonValue_.getMemberNames();};

    Json::Value & operator[] (const char *key) 
        {return m_jsonValue_[key];};

    const Json::Value & operator[] (const char *key) const 
        {return m_jsonValue_[key];};

    Json::Value & operator[] (const std::string &key) 
        {return m_jsonValue_[key];};

    const Json::Value & operator[] (const std::string &key) const 
        {return m_jsonValue_[key];};

    Json::Value & operator[] (const Json::StaticString &key) 
        {return m_jsonValue_[key];};

    void clear();
    ////////////////////////////////////////////////////////////////
    //Abov  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////

    /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as operator[](), except it will
     * create an index for this property to optimize the search (find methods).
     * 
     * Notes:
     * 1. the index created is based on the type of the value, so that only 
     *      those beans that have the same property type will be returned.
     * 
     * @param name name of the property
     * @param value value of the property
     * @return The pid of the property if success, or an error code.
     *                   This is useful when the property is firstly set.
     *                   error code:
     *                   -1: if name is nullptr or empty, or the value is null
     */
    pidType setProperty(const char* name, const Json::Value& value);
    
     /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as operator[](), except it will
     * create an index for this property to optimize the search.
     * 
     * Notes:
     * 1. This method is aimed to achieve better performance. It is highly 
     *      encouraged to use this one if the property has already been set
     *      before.
     * 
     * @param pid id of the property
     * @param value value of the property
     * @return The pid of the property if success, or an error code.
     *                   error code:
     *                   -1: if the value is null
     *                   -2: if the property with the pid does not exist.
     */
    pidType setProperty(pidType pid, const Json::Value& value);

    /**
     * Remove property from this bean
     * @param name the property name
     * @return the removed item as json value
     */
    Json::Value removeProperty( const char* name);

    /**
     * Remove property from this bean
     * @param pid the property id
     * @return the removed item as json value
     */
    Json::Value removeProperty(pidType pid);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;
    virtual ~Bean();

    // void swap(Json::Value &other);

    Json::Value m_jsonValue_;
    BeanWorld* m_world_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;

    oidType m_id_ = 0;
    // std::string m_name_;
    // std::string uri_;

friend class BeanWorld;
};

}
}
}