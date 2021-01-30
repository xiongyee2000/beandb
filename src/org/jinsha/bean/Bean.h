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
    enum PTYPE
    {
        INVALID = 0,
        BOOL = 10,
        CHAR,
        UCHAR,
        INT,
        UINT,
        LONG,
        ULONG,
        STR,
        FLOAT,
        DOUBLE,
        LIST = 100,
        BEAN = 200
    };

    // Bean() {};

    // otype getClassId() {return classId_;};
    // std::string getClassName() {return className_;};
    // std::string getClassUri() {return classUri_;};

    otype getId() {return m_id_;};
    std::string &getName() {return m_name_;};
    // std::string &getUri() {return uri_;};

    // void setClassId(otype id) {classId_ = id;};
    // void setClassName(const std::string& name) {className_ = name;};
    // void setClassUri(const std::string& uri) {classUri_ = uri;};

    void setId(otype id) {m_id_ = id;};
    void setName(const std::string& name) {m_name_ = name;};
    // void setUri(const std::string& uri) {uri_ = uri;};

    /**
     * Define a property for this bean. 
     * 
     * @param name name of the property
     * @param value value of the property
     * @return 0 if success, or an error code
     * error code:
     *      -1 if name is nullptr or empty
     */
    int setProperty(const char* name, char value);
    int setProperty(const char* name, unsigned char value);
    int setProperty(const char* name, int value);
    int setProperty(const char* name, unsigned int value);
    int setProperty(const char* name, Json::Value::Int64 value);
    int setProperty(const char* name, const char* value);
    int setProperty(const char* name, bool value);

    /**
     * Undefine a property for this bean. 
     * 
     * Note the property value will be removed once the property is 
     * undefined.
     * 
     * @param name name of the property 
     */
    // void removeProperty(const std::string& name); 
    void removeProperty(const char* name); 

private:
    Bean(BeanWorld* world) : m_world_(world) {};
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;

    BeanWorld* m_world_;

    // otype classId_ = 0;
    // std::string className_;
    // std::string classUri_;

    otype m_id_ = 0;
    std::string m_name_;
    // std::string uri_;

    std::unordered_map<int, otype> longPropertyMap_;

friend class BeanWorld;
};

}
}
}