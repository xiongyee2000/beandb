#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "jsoncpp/json/value.h"

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
    Json::Value get (const char *key, const Json::Value &defaultValue) const {return m_jsonValue_.get(key, defaultValue);};
    Json::Value get (const std::string &key, const Json::Value &defaultValue) const {return m_jsonValue_.get(key, defaultValue);};
    bool isMember (const std::string &key) const {return m_jsonValue_.isMember(key);};
    Json::Value::Members getMemberNames () const {return m_jsonValue_.getMemberNames();};
    Json::Value & operator[] (const char *key) {return m_jsonValue_[key];};
    const Json::Value & operator[] (const char *key) const {return m_jsonValue_[key];};
    Json::Value & operator[] (const std::string &key) {return m_jsonValue_[key];};
    const Json::Value & operator[] (const std::string &key) const {return m_jsonValue_[key];};
    Json::Value & operator[] (const Json::StaticString &key) {return m_jsonValue_[key];};

    /**
     * same as removeProperty()
     */
    Json::Value removeMember (const char *key);
    Json::Value removeMember (const std::string &key);


    void clear();
    ////////////////////////////////////////////////////////////////
    //Abov  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////

    /**
     * Set the value of a property of this bean. 
     * 
     * Notes:
     * 1. each property is bound to a fixed type, which will be determined
     *     by the value argment given in the first invocation of this  method;
     * 3. Invocation of this method with a different value type will cause
     *     this method return an error code.
     * 
     * @param name name of the property
     * @param value value of the property
     * @return the pid of the property if success, or an error code
     * error code:
     *      -1 if name is nullptr or empty, or the value is nullptr
     */
    // int setProperty(const char* name, const Json::Value& value);
    pidType setProperty( const char* name, bool value);
    pidType setProperty( const char* name, Json::Int value);
    pidType setProperty( const char* name, Json::UInt value);
    pidType setProperty( const char* name, Json::Int64 value);
    pidType setProperty( const char* name, Json::UInt64 value);
    pidType setProperty( const char* name, double value);
    pidType setProperty( const char* name, const char* value);

    /**
     * Remove property from this bean
     * @param name the property name
     * @return the removed item as json value
     */
    Json::Value removeProperty( const char* name);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;
    virtual ~Bean();

    // void swap(Json::Value &other);

    Json::Value m_jsonValue_;


    // template<typename T>
    // int setProperty(const char* name,  const T& value)
    // {
    //     Json::Value v(v);
    //     return doSetProperty(name, v);
    // }

    // int doSetProperty(const char* name, const Json::Value& value);

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