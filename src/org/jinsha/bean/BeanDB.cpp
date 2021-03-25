
#include "./BeanDB.h"


namespace org {
namespace jinsha {
namespace bean {

BeanDB::BeanDB(BeanWorld& world,  const char* dir) : 
    m_world_(&world), m_dir_(dir)
{

}

int BeanDB::loadAll()
{
    return 0;
}


int BeanDB::saveAll()
{
    return 0;
}


Bean* BeanDB::loadBean(oidType id)
{
    return 0;

}


int BeanDB::saveBean(Bean* bean)
{
    return 0;
}


int removeBean(Bean* bean)
{
    return 0;
}

int loadProperties()
{
    return 0;
}

int saveProperty(Property* property)
{
    return 0;
}


int removeProperty(Property* property)
{
    return 0;
}


}
}
}