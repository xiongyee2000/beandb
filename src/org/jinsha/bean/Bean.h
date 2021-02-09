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

class Bean : public Json::Value
{
public:
    // otype getClassId() {return classId_;};
    // std::string getClassName() {return className_;};
    // std::string getClassUri() {return classUri_;};

    oidType getId() {return m_id_;};
    // std::string &getName() {return m_name_;};
    // std::string &getUri() {return uri_;};

    // void setClassId(otype id) {classId_ = id;};
    // void setClassName(const std::string& name) {className_ = name;};
    // void setClassUri(const std::string& uri) {classUri_ = uri;};

    // void setId(otype id) {m_id_ = id;};
    // void setName(const std::string& name) {m_name_ = name;};
    // void setUri(const std::string& uri) {uri_ = uri;};

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
     * @return 0 if success, or an error code
     * error code:
     *      -1 if name is nullptr or empty
     */
    // int setProperty(const char* name, const Json::Value& value);
    int setProperty( const char* name, bool value);
    int setProperty( const char* name, Json::Int value);
    int setProperty( const char* name, Json::UInt value);
    int setProperty( const char* name, Json::Int64 value);
    int setProperty( const char* name, Json::UInt64 value);
    int setProperty( const char* name, double value);
    int setProperty( const char* name, const char* value);

    /**
     * Remove a property from this bean. 
     * 
     * Note the property value will be removed once the property is 
     * undefined.
     * 
     * @param name name of the property 
     */
    // void removeProperty(const std::string& name); 
    void removeProperty(const char* name); 

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;
    virtual ~Bean();


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