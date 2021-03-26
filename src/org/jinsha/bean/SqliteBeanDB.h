#pragma once

#include "./AbstractBeanDB.h"

namespace org {
namespace jinsha {
namespace bean {

class SqliteBeanDB : public AbstractBeanDB
{
public:
    SqliteBeanDB(BeanWorld& world, const char* dir);
    ~SqliteBeanDB() override;

    int loadAll() override;
    int saveAll() override;

    Bean* loadBean(oidType id) override;
    int saveBean(Bean* bean) override;
    int removeBean(Bean* bean) override;

    int loadProperties() override;
    int saveProperty(Property* property) override;
    int removeProperty(Property* property) override;

private:
    std::string m_dir_;
};

}
}
}