

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/BeanWorld.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

void basic()
{
    BeanWorld *world = new BeanWorld();
    EXPECT_TRUE(nullptr != world);
    EXPECT_TRUE(world->getNumOfBeans() == 0);
    EXPECT_TRUE(world->getProperties().size() == 0);

    EXPECT_TRUE(world->getNumOfBeans() == 0);

    Bean *bean1 = world->createBean();
    EXPECT_TRUE(nullptr != bean1);
    EXPECT_TRUE(world->getNumOfBeans() == 1);

    Bean *bean2 = world->createBean();
    EXPECT_TRUE(nullptr != bean2);
    EXPECT_TRUE(world->getNumOfBeans() == 2);

    Bean *bean3 = world->createBean();
    EXPECT_TRUE(nullptr != bean3);
    EXPECT_TRUE(world->getNumOfBeans() == 3);

    world->removeBean(bean1->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 2);

    world->removeBean(bean2->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 1);

    world->removeBean(bean3->getId());
    EXPECT_TRUE(world->getNumOfBeans() == 0);

    delete world;
}

TEST(BeanWorld, Constructor)
{
    basic();
}

TEST(BeanWorld, createBean_removeBean)
{
    basic();
}

TEST(BeanWorld, getNumOfBeans)
{
    basic();
}

TEST(BeanWorld, getBeans)
{
    BeanWorld *world = new BeanWorld();

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    auto& beans  = world->getBeans();
    EXPECT_TRUE(beans.size() == 3);

    delete world;
}

TEST(BeanWorld, getBean)
{
    BeanWorld *world = new BeanWorld();
    Bean* bean_ = nullptr;

    std::vector<int>::const_iterator it;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    bean_ = world->getBean(bean1->getId());
    EXPECT_TRUE(bean_ == bean1);
    bean_ = world->getBean(bean2->getId());
    EXPECT_TRUE(bean_ == bean2);
    bean_ = world->getBean(bean3->getId());
    EXPECT_TRUE(bean_ == bean3);

    bean_ = world->getBean(555);
    EXPECT_TRUE(bean_ == nullptr);

    delete world;
}

TEST(BeanWorld, clear)
{
    BeanWorld *world = new BeanWorld();

    world->createBean();
    world->createBean();
    world->createBean();
    EXPECT_TRUE(world->getNumOfBeans() == 3);

    world->clear();
    EXPECT_TRUE(world->getNumOfBeans() == 0);

    delete world;
}

TEST(BeanWorld, defineProperty_undefineProperty)
{
    BeanWorld world;
    Value value;
    const Property* property = nullptr;

    property = world.defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world.defineArrayProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world.defineRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world.defineArrayRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty("p1");
    EXPECT_TRUE(nullptr == world.getProperty("p1"));
}

TEST(BeanWorld, getProperty)
{
    BeanWorld world;
    Value value;
    int pid = 0;
    const Property* property = nullptr;

    init_world(world);

    Bean *bean = world.createBean();
    property = world.getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("a");
    EXPECT_TRUE(property == nullptr);

    property = world.defineProperty("p1", Property::IntType);
    property = world.getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");

    property = ((const BeanWorld*)&world)->getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");
}

TEST(BeanWorld, getProperties)
{
    BeanWorld *world = new BeanWorld();
    Value value;
    Property *p1, *p2;

    // init_world(*world);

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    p1 = world->defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty("", Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty(string("").c_str(),Property::IntType);
    EXPECT_TRUE(p1 == nullptr);
    EXPECT_TRUE(world->getProperties().size() == 0);

    p1 = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(p1 == world->getProperty("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    p1 = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(p1 == world->getProperty("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    p2 = world->defineProperty("p2", Property::IntType);
    EXPECT_TRUE(p2 == world->getProperty("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean1->setProperty(p1, 1);
    bean2->setProperty(p1, 2);

    world->undefineProperty("p1");
    EXPECT_TRUE(world->getProperties().size() == 1);
    EXPECT_TRUE(bean1->hasProperty(p1) == false);
    world->undefineProperty("p2");
    EXPECT_TRUE(world->getProperties().size() == 0);
    EXPECT_TRUE(bean2->hasProperty(p1) == false);
    delete world;
}

TEST(BeanWorld, removeBean)
{
    BeanWorld world;
    Value value;
    oidType oid = 0;
    Bean* bean = world.createBean();
    oid = bean->getId();
    world.removeBean(bean->getId());
    EXPECT_TRUE(world.getBean(oid) == nullptr);
}
