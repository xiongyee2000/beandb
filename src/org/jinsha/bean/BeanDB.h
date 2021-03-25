#pragma once

#include "./BeanWorld.h"

namespace org {
namespace jinsha {
namespace bean {

class BeanDB
{
public:
    BeanDB(BeanWorld& world, const char* dir);

    int loadAll();
    int saveAll();

    Bean* loadBean(oidType id);
    int saveBean(Bean* bean);
    int removeBean(Bean* bean);

    int loadProperties();
    int saveProperty(Property* property);
    int removeProperty(Property* property);

private:
    std::string m_dir_;
    BeanWorld *m_world_;
};

}
}
}