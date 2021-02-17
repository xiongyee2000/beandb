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
    int getPropertyId(const char* name) const;

    /**
     * Get property by id.
     * 
     * @param pid property id
     * @return property
     */
    const Property* getProperty(pidType pid) const;

    /**
     * Get property by name.
     * 
     * @param name property name
     * @return property
     */
    const Property* getProperty(const char* name) const;

    /**
     * Get all properties.
     * 
     * @return a map containing all properties.
     */
    const std::unordered_map<std::string, unsigned int>& getProperties() const 
     {return m_propertyMap_;};

    /**
     * Create index for a property, which can be used to improve
     * search performance.
     * 
     * Note this method will create index entries by checking 
     * each bean's properties: if it has this property (member) then
     * create an entry for it in the index, otherwise skip. This results 
     * the time complexity of this method O(n) where n is the number 
     * of beans in this world.
     * 
     * If index is desired, It is highly sugguested to call this method 
     * before setting any  value for a property so as to achieve the 
     * best performance.
     * 
     * @param propertyName the name of the property
     * @return the property id, or error code:
     *                   -1: if name is nullptr or empty
     *                   -2: if the index already exists
     */
    pidType createIndex(const char* propertyName);
    pidType removeIndex(pidType pid);

    /**
     * Update index for a property.
     * 
     * @param pid the id of the property
     * @return the property id, or -1 if pid is invalid
     */
    pidType updateIndex(Property* property);

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

    // void findEqual(pidType pid,  const Json::Value& value, std::list<Bean*>& beans);
    // void findLessEqual(pidType pid,  const Json::Value& value, std::list<Bean*>& beans);
    // void findGreaterEqual(pidType pid,  const Json::Value& value, std::list<Bean*>& beans);
    // void findLessThan(pidType pid,  const Json::Value& value, std::list<Bean*>& beans);
    // void findGreaterThan(pidType pid,  const Json::Value& value, std::list<Bean*>& beans);

private:
    pidType setProperty( Bean* bean, const char* name, const Json::Value&  value);
    void setProperty( Bean* bean, pidType pid, const Json::Value&  value);
    void doSetProperty( Bean* bean, Property* property, const Json::Value&  value);
    pidType addProperty(const char* name);
    Json::Value removeProperty(Bean* bean, const char* name);
    Json::Value removeProperty(Bean* bean, Property* property);
    void removeProperty(pidType pid);

    void findCommon(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);
    void trivialFind(int opType, const char* propertyName,  const Json::Value& value, std::list<Bean*>& beans);

    int addRelation(Bean* from, Bean* to, const char* propertyName);
    // int addRelation(Bean* from, Bean* to, const char* propertyName);
    int removeRelation(Bean* from, Bean* to, const char* propertyName);

    oidType generateBeanId();

private:
    std::unordered_map<oidType, Bean*> m_beans_;
    oidType m_maxBeanId_ = 0;

    std::vector<Property*> m_properties_;
    //map from property name to index
    std::unordered_map<std::string, unsigned int> m_propertyMap_; 
friend class Bean;

};

}
}
}