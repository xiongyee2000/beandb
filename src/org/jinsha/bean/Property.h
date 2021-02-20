#pragma once

#include <string>
#include <list>
#include <unordered_map>

#include "common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

class Property
{
public:
    /**
     * Get the name of the property
     * 
     * @return property name (key)
     */
    const std::string& getName() const {return m_name_;};

    /**
     * Create index for a property, which can be used to improve
     * search performance.
     * 
     * Notes
     * This method will create index entries by checking 
     * each bean's properties: if it has this property (member) then
     * create an entry for it in the index, otherwise skip. (This may result 
     * the time complexity of this method O(n) where n is the number 
     * of beans in this world.)
     * And once index is created, the index will be updated each time
     * when Bean::setProperty() method is called.
     * You can call this method at any time. However If index is desired 
     * for a property, It is strongly  sugguested to call this method right 
     * after the first time this property is set by Bean::setProperty() . 
     * This can help to achieve the best performance.
     * 
     * @return 0 for success, or -1 if the index already exists
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
    /**
     * Constructor
     * 
     * @param name property name (similar to json key)
     */
    Property(BeanWorld* world, const char* name) : 
        m_world_(world), m_name_(name) {};
    Property(const char* name) : m_name_(name) {};
    virtual ~Property();

private:
    void addIndex(Bean* bean, const Json::Value& value);
    bool removeIndex(Bean* bean, const Json::Value& value);

    void findCommon(int type, const Json::Value& value, std::list<Bean*>& beans) const; 

private:
    BeanWorld* m_world_;
    std::string m_name_;
    pidType m_pid_ = -1;
    unsigned int m_refCount_ = 0;
    bool m_indexed_ = false;
    std::unordered_map<oidType, Bean*> m_trueValueMap_;
    std::unordered_map<oidType, Bean*> m_falseValueMap_;
    std::multimap<int_t, Bean*> m_intValueMap_;
    std::multimap<uint_t, Bean*> m_uintValueMap_;
    std::multimap<double, Bean*> m_doubleValueMap_;

     struct StrComparator
    {
        bool operator()(const char* const & a, const char* const & b) const;
    };
    std::multimap<const char*, Bean*, StrComparator> m_strValueMap_;


friend class BeanWorld;
};

}
}
}