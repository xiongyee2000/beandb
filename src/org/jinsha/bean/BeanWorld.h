#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class Property;

class BeanWorld
{
public:
    BeanWorld();
    virtual ~BeanWorld();

    void clear();

    /**
     *  Create a bean.
     * 
     * @return the created bean
     */
    Bean *createBean();

    /**
     * Remove a bean.
     * 
     * @param id the id of the bean
     */
    void removeBean(oidType id);

    /**
     * Get total number of beans.
     * 
     * @return the total number of beans
     */
    int getNumOfBeans();

    /**
     * Get all beans.
     * 
     * @return a map containing all beans
     */
    const std::unordered_map<oidType, Bean*>& getBeans();

    /**
     * Get bean by id.
     * 
     * @param id the bean id
     * @return the bean, or nullptr
     */
    Bean* getBean(oidType id);

    /**
     * Define a property.
     * 
     * Property must be defined before it can be used.
     * 
     * Notes:
     * - Property is type sensitive: i.e. setting the property with a value of type other
     *    than the one defined here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of property
     * @return the id of the property, or error code:
     *                   -1: name is null or empty
     *                  -2: the property is already defined
     */
    pidType defineProperty(const char* name, Property::ValueType valueType);

    /**
     * Define an array property.
     * 
     * Array property must be defined before it can be used.
     * 
     * Notes:
     * - Array property is type sensitive: i.e. adding to the array property 
     *   with a value of type other than the one defined here will fail.
     * 
     * @param name the name of property
     * @param valueType the value type of the element of the array property
     * @return the id of the property, or error code:
     *                   -1: name is null or empty
     *                  -2: the property is already defined
     */
    pidType defineArrayProperty(const char* name, Property::ValueType valueType);

    /**
     * Define a relation property.
     * 
     * Relation is a special kind of property, which represents relationship between
     * two beans, e.g. father/mother etc.
     * 
     * @param name the name of relation property
     * @return the id of the relation property, or error code:
     *                   -1: name is null or empty
     *                  -2: the relation property is already defined
     */
    pidType defineRelation(const char* name);

    /**
     * Define an array relation property.
     * 
     * Array relation property must be defined before it can be used.
     * 
     * @param name the name of array relation property
     * @return the id of the array relation property, or error code:
     *                   -1: name is null or empty
     *                  -2: the array relation property is already defined
     */
    pidType defineArrayRelation(const char* name);

    /**
     * Undefine a property.
     * 
     * Notes:
     * - This method can be used to undefine either a property, an array property,
     *    a relation property, or an array relation property;
     * - All beans that have this property will remove the property with this id.
     * 
     * @param name the name of property
     * @return 
     */
    void undefineProperty(const char* name);

    /**
     * Get the id of a property. Later the property id 
     * can be used in other methods.
     * 
     * It is highly encouraged to use property id, for it
     * can reduce a lot of string calculation and achieve 
     * higher performance.
     *  
     * @param property name
     * @return property id
     */
    pidType getPropertyId(const char* name) const;

    /**
     * Get property by id.
     * 
     * @param id property id
     * @return property
     */
    const Property* getProperty(pidType id) const;
    Property* getProperty(pidType id);

    /**
     * Get property by name.
     * 
     * @param name property name
     * @return property
     */
    const Property* getProperty(const char* name) const;
    Property* getProperty(const char* name);

    /**
     * Get all properties.
     * 
     * @return a map containing all properties.
     */
    const std::unordered_map<std::string, pidType>& getProperties() const 
     {return m_propertyMap_;};

    /**
     * Get relation by name.
     * 
     * @param name relation name
     * @return the relation, or null
     */
    const Property* getRelation(const char* name) const;
    Property* getRelation(const char* name);

    /**
     * Find beans whose property values are equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are less equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are greater equal to the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterEqual(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are less than the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findLessThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    /**
     * Find beans whose property values are greater than the given one.
     * 
     * @param propertyName the name of the property
     * @param value the value of the property
     * @param beans the results
     * 
     * Notes:
     * 1. the search is type restricted, i.e. only those beans with the property value
     *     having the same type will be considered. 
     */
    void findGreaterThan(const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

private:
    pidType doDefineProperty(const char* name, Property::Type type, 
    Property::ValueType valueType, bool createIndex = false);

    int setProperty( Bean* bean, Property* property, const Json::Value&  value);
    int setArrayProperty( Bean* bean, Property* property, Json::Value::ArrayIndex index, const Json::Value&  value);
    void doSetProperty(Bean* bean, Property* property, Json::Value* oldValue,  const Json::Value&  newValue);

    void setRelation(Property* property, Bean* from, Bean* to);
    void setArrayRelation(Property* property, Bean* from, Json::Value::ArrayIndex index, Bean* to);
    Json::Value removeProperty(Bean* bean, Property* property);

    void setRelation( Bean* bean, pidType id, const Json::Value&  value);
    void doSetRelation( Bean* bean, Property* property, const Json::Value&  value);

    void recreateIndex(Property* property);

    void findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    oidType generateBeanId();

private:
    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 0;

    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, pidType> m_propertyMap_; 

friend class Bean;
friend class Property;
};

}
}
}