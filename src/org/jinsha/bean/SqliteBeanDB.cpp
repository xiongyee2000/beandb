
#include "./SqliteBeanDB.h"


namespace org {
namespace jinsha {
namespace bean {

SqliteBeanDB::SqliteBeanDB(BeanWorld& world,  const char* dir) : 
    AbstractBeanDB(world), m_dir_(dir)
{

}


SqliteBeanDB::~SqliteBeanDB()
{
}


int SqliteBeanDB::loadAll()
{
    return 0;
}


int SqliteBeanDB::saveAll()
{
    return 0;
}


Bean* SqliteBeanDB::loadBean(oidType id)
{
    return 0;

}


int SqliteBeanDB::saveBean(Bean* bean)
{
    return 0;
}


int SqliteBeanDB::removeBean(Bean* bean)
{
    return 0;
}

int SqliteBeanDB::loadProperties()
{
    return 0;
}

int SqliteBeanDB::saveProperty(Property* property)
{
    return 0;
}


int SqliteBeanDB::removeProperty(Property* property)
{
    return 0;
}


}
}
}