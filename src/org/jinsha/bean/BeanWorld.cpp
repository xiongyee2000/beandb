
#include <assert.h>
#include "./BeanWorld.h"

using namespace std;

namespace org {
namespace jinsha {
namespace bean {

BeanWorld::~BeanWorld()
{
    clear();
};

void BeanWorld::clear()
{
    for (auto item : m_beans_) 
    {
        delete item;
    }
    m_beans_.clear();
}

Bean* BeanWorld::createBean()
{
    Bean* bean = new Bean(this);
    assert(bean);
    bean->m_id_ = generateObjectId();
    m_beans_.push_back(bean);
    return bean;
};

bool BeanWorld::hasProperty(const char* name)
{
    if (name == nullptr) return false;
    for (const auto& property : m_properties_)
    {
        if (property->getName().compare(name) == 0) 
        {
            return true;
        } 
    }
    return false;
}

void BeanWorld::addProperty(const char* name)
{
    if (!hasProperty(name))
    {
        auto property = new Property(name);
        m_properties_.push_back(property);
    }
}

void BeanWorld::removeProperty(const char* name)
{
    for (auto iter = m_properties_.begin();  iter != m_properties_.end(); iter++)
    {
        if ((*iter)->getName().compare(name) == 0) 
        {
            delete (*iter);
            m_properties_.erase(iter);
            return;
        } 
    }
}

otype BeanWorld::generateObjectId()
{
    //todo
    return 0;
};

}
}
}