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
    //Above  are corresponding Json::Value methods
    ////////////////////////////////////////////////////////////////

    /**
     * Get property value.
     * 
     * @param name the property name
     * @return the property value
     * 
     * Note:
     * If this bean does not have property with name,
     * a json value of null will be returned.
     */
    Json::Value getProperty(const char* name) const;

    /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as bean['key'] = value, excpect
     * that the index will be updated if the property is indexed. But also
     * note that index won't apply to property values of list/object type.
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
     *                   -1: if the property with the name does not exist
     *                   -2: if the value is null
     */
    pidType setProperty(const char* name, const Json::Value& value);
    
     /**
     * Set the value of a property of this bean. 
     * 
     * This method is basically the same as bean['key'] = value, excpect
     * that the index will be updated if the property is indexed. But also
     * note that index won't apply to property values of list/object type.
     * 
     * @param pid id of the property (see BeanWorld::getPropertyId())
     * @param value value of the property
     * @return The pid of the property if success, or an error code.
     *                   error code:
     *                   -1: if the property with the id does not exist
     *                   -2: if the value is null
     */
    pidType setProperty(pidType pid, const Json::Value& value);

    /**
     * Is the specified property an array property.
     * 
     * @param name the property name
     * @return true if it is an array property, false otherwise
     * 
     * Note:
     * If this bean does not have property with name,
     * false will be returned;
     * If the current property value is not a json array, 
     * false will be returned;
     */
    bool isArrayProperty(const char* name) const;

    /**
     * Get size of an array property.
     * 
     * @param name the property name
     * @return the size
     * 
     * Note:
     * If this bean does not have property with name,
     * 0 will be returned;
     * The current property value must be a json array, 
     * or 0 will be returned;
     */
    Json::Value::ArrayIndex getPropertySize(const char* name) const;

    /**
     * Get value of an array property at specified index.
     * 
     * @param name the property name
     * @param index the index in the array
     * @return the property value
     * 
     * Note:
     * If this bean does not have property with name,
     * a json value of null will be returned;
     * The current property value must be a json array, or a json 
     * value of null will be returned;
     * If the index is invalid, a json value of null will be returned;
     */
    Json::Value getProperty(const char* name, 
        Json::Value::ArrayIndex index) const;

     /**
     * Set the value of an array property at specified index. 
     * 
     * Note this method must be called after the array property
     * is created, say be setProperty() with  json value of array type.
     * 
     * @param name name of the property
     * @param index the index in the array
     * @param value value to be set
     * @return 0 if success, or an error code.
     *                   error code:
     *                   -1: if the property with the name does not exist
     *                   -2: if the value is null
     *                   -3: if he current property value is not a json array
     *                   -4: if the index is invalid
     */
    int setProperty(const char* name, Json::Value::ArrayIndex index,
        const Json::Value& value);

     /**
     * Append an item to the end of an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return 0 if success, or an error code.
     *                   error code:
     *                   -1: if the property with the name does not exist
     *                   -2: if the value is null
     *                   -3: if he current property value is not a json array
     */
    int appendProperty(const char* name, const Json::Value& value); 

     /**
     * Resize an array property. 
     * 
     * @param name name of the property
     * @param value value to be added
     * @return The pid of the property if success, or an error code.
     *                   error code:
     *                   -1: if the property with the name does not exist
     *                   -3: if he current property value is not a json array
     */
    pidType resizeProperty(const char* name, Json::Value::ArrayIndex size);        

    /**
     * Remove property from this bean.
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