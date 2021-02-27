#pragma once

#include <string>
#include "common.h"

namespace org {
namespace jinsha {
namespace bean {

class Bean;
class BeanWorld;

class Relation
{
public:
    /**
     * Get the name of the relation
     * 
     * @return relation name
     */
    const std::string& getName() const {return m_name_;};

private:
    /**
     * Constructor
     * 
     * @param name relation name
     */
    Relation(BeanWorld* world, const char* name) : 
        m_world_(world), m_name_(name) {};
    Relation(const char* name) : m_name_(name) {};
    virtual ~Relation();

    int createIndex() {return 0;};
    int addIndex(Bean* bean, const Json::Value& value) {return 0;};
    int removeIndex(Bean* bean, const Json::Value& value) {return 0;};
    bool indexed() const {return m_indexed_;};


private:
    BeanWorld* m_world_;
    std::string m_name_;
    pidType m_pid_ = -1;
    unsigned int m_refCount_ = 0;
    bool m_indexed_ = true;

friend class BeanWorld;
};

}
}
}