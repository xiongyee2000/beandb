#pragma once

// #include <memory>
#include <list>

#include "./common.h"
#include "./Property.h"
#include "./Bean.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanWorld
{
public:
    BeanWorld() {};
    virtual ~BeanWorld();

    void clear();
    const std::list<Property*> & getProperties() const {return m_properties_;};
    bool hasProperty(const char* name);

    Bean *createBean();

private:
    void addProperty(const char* name);
    void removeProperty(const char* name);

    otype generateObjectId();

    std::list<Bean*> m_beans_;
    std::list<Property*> m_properties_;

friend class Bean;
};

}
}
}