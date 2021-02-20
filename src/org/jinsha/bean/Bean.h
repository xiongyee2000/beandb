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

    bool isMember (const char* key) const;
    bool isMember (const std::string &key) const;

    Json::Value::Members getMemberNames () const 
        {return m_jsonValue_.getMemberNames();};

    void clear();
    ////////////////////////////////////////////////////////////////
    //Abov  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////

    /**
     * Get property value.
     * 
     * @param propertyName the property name
     * @return the property value
     * 
     * Note:
     * If this bean does not have property with propertyName,
     * a json value of null will be returned.
     */
    Json::Value getProperty(const char* propertyName) const;

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
     * This method is basically the same as bean['key'] = value, 
     * except that it will create an index for this property to 
     * optimize the search.
     * 
     * Notes:
     * 1. This method is aimed to achieve better performance. It is highly 
     *      encouraged to use this one if the property has already been set
     *      before.
     * 
     * @param pid id of the property (see BeanWorld::getPropertyId())
     * @param value value of the property
     * @return The pid of the property if success, or an error code.
     *                   error code:
     *                   -1: if the value is null
     *                   -2: if the property with the pid does not exist.
     */
    pidType setProperty(pidType pid, const Json::Value& value);

    /**
     * Remove property from this bean.
     * 
     * Note the property will be removed no matter is was previously
     * set by setProperty() or by bean['key'] = value .
     * 
     * @param name the property name
     * @return the removed item as json value
     */
    Json::Value removeProperty( const char* name);

    /**
     * Remove property from this bean.
     * 
     * Note the method is only effective when the pid is valid, 
     * i.e. the property was previously set by setProperty().
     * 
     * @param pid the property id
     * @return the removed item as json value, or null value if
     *                   the pid is invalid.
     */
    Json::Value removeProperty(pidType pid);

private:
    Bean(BeanWorld* world);
    Bean(const Bean& bean) = delete;
    Bean& operator =(const Bean& bean) = delete;
    virtual ~Bean();

    const Json::Value& get(const char* key);

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