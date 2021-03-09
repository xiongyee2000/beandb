#pragma once

#include <string>
#include <list>
#include <map>

#include "common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

class Property
{
public:
    enum Type
    {
        /**
         * the property value is a primary type, i.e. ValueType
         */
        PrimaryType = 0,

        /**
         * the property is an array, the element of which is of ValueType
         */
        ArrayPrimaryType,

        /**
         * the property represents a relation between two beans
         */
        RelationType,

        /**
         * the property represents an array of relations between one and
         * multiple beans
         */
        ArrayRelationType
    };

    enum ValueType
    {
        IntType = Json::intValue,
        UIntType = Json::uintValue,
        RealType = Json::realValue,
        StringType = Json::stringValue,
        BoolType = Json::booleanValue
    } ;

public:
    /**
     * Get the name of the property
     * 
     * @return property name (key)
     */
    const std::string& getName() const {return m_name_;};

    /**
     * Get the value type of this property.
     */
    ValueType getValueType() const {return m_valueType_;};

    /**
     * Get the type of this property.
     */
    Type getType() const {return  m_propertyType_ ;};

    /**
     * Create index for a property, which can be used to improve
     * search performance.
     * 
     * Notes
     * - This method will create index entries by checking 
     *    each bean's properties: if it has this property (member) then
     *    create an entry for it in the index, otherwise skip. (This may result 
     *    the time complexity of this method O(n) where n is the number 
     *    of beans in this world.)
     *    And once index is created, the index will be updated each time
     *    when Bean::setProperty() method is called.
     * - You can call this method at any time. However If index is desired 
     *    for a property, It is strongly  sugguested to call this method right 
     *    after the first time this property is set by Bean::setProperty() . 
     *    This can help to achieve the best performance.
     * - Currently index is supported only on property of PrimaryType
     * 
     * @return 0 if success, or  an error code
     *                   -1: unable to create index
     */
    int createIndex();

    /**
     * Remove index for a property.
     * 
     * Note this method will delete all index entries, which may result 
     * the time complexity of this method O(n) where n is the number 
     * of beans in this world.
     * 
     * @return 0 for success, or -1 if the index doesn't exist
     */
    int removeIndex();

    /**
     * Is property indexed.
     * 
     * @return true if indexed, false otherwise
     */
    bool indexed() const {return m_indexed_;};

private:
    Property(BeanWorld* world, const char* name, 
        Type type, ValueType valueType, 
        bool needIndex = false) : 
        m_world_(world), m_name_(name), 
        m_propertyType_(type), m_valueType_(valueType), 
        m_indexed_(needIndex) {};
    // Property(const char* name) : m_name_(name) {};
    virtual ~Property();

private:
    void addBean(Bean* bean); //used only for array property/relation
    void removeBean(Bean* bean); //used only for array property/relation

    void addIndex(Bean* bean, const Json::Value& value);
    bool removeIndex(Bean* bean, const Json::Value& value);

    void findHas(std::list<Bean*>& beans) const;
    std::list<oidType>&& findSubjects(oidType objectId);

    void findCommon_(int type, const Json::Value& value, std::list<Bean*>& beans) const; 

private:
    BeanWorld* m_world_;
    std::string m_name_;
    pidType m_id_ = -1;
    Type m_propertyType_;
    ValueType m_valueType_ ;
    unsigned int m_refCount_ = 0;
    bool m_indexed_ = false;

    //keep all beans that have this property
    //used only for array property and array relation
    //for better performance
    std::map<Bean*, unsigned int> m_beanMap_; 

    //index for true values
    std::map<oidType, Bean*> m_trueValueMap_;

    //index for false values
    std::map<oidType, Bean*> m_falseValueMap_;

    //index for int values
    std::multimap<int_t, Bean*> m_intValueMap_;

    //index for uint values
    std::multimap<uint_t, Bean*> m_uintValueMap_;

    //index for double values
    std::multimap<double, Bean*> m_doubleValueMap_;

     struct StrComparator
    {
        bool operator()(const char* const & a, const char* const & b) const;
    };
    std::multimap<const char*, Bean*, StrComparator> m_strValueMap_;


friend class BeanWorld;
friend class Bean;
};

}
}
}