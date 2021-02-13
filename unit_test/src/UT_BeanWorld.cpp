

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/BeanWorld.h"

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
    Bean* bean_ = nullptr;

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

TEST(BeanWorld, getProperty)
{
    BeanWorld world;
    Value value;
    int pid = 0;
    const Property* property = nullptr;

    Bean *bean = world.createBean();
    property = world.getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("a");
    EXPECT_TRUE(property == nullptr);

    bean->setProperty("p1", 1);
    property = world.getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");
    (*bean)["p2"] = 2;
    property = world.getProperty("p2");
    EXPECT_TRUE(property == nullptr);
}

TEST(BeanWorld, getProperties)
{
    BeanWorld *world = new BeanWorld();
    Value value;
    int pid = 0;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    pid = bean1->setProperty(nullptr, 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = bean1->setProperty("", 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = bean1->setProperty(string("").c_str(), 1);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = bean1->setProperty("p1", nullptr);
    EXPECT_TRUE(pid == -1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    pid = bean1->setProperty("p1", 8);
    EXPECT_TRUE(pid == world->getPropertyId("p1"));
    EXPECT_TRUE(world->getProperties().size() == 1);

    pid = bean1->setProperty("p2", 8);
    EXPECT_TRUE(pid == world->getPropertyId("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    pid = bean2->setProperty("p1", "v1");
    EXPECT_TRUE(pid == world->getPropertyId("p1"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    pid = bean2->setProperty("p2", "v2");
    EXPECT_TRUE(pid == world->getPropertyId("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    pid = bean2->setProperty("p2",  2);
    EXPECT_TRUE(pid == world->getPropertyId("p2"));
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean1->removeMember("p1");
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean2->removeMember("p1");
    EXPECT_TRUE(world->getProperties().size() == 1);

    bean1->removeMember("p2");
    EXPECT_TRUE(world->getProperties().size() == 1);

    bean2->removeMember("p2");
    EXPECT_TRUE(world->getProperties().size() == 0);

    delete world;
}

TEST(BeanWorld, getPropertyId)
{
    BeanWorld *world = new BeanWorld();
    Value value;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    bean1->setProperty("p1", 8);
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean1->setProperty("p2", 8);
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean2->setProperty("p1", "v1");
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean2->setProperty("p2", "v2");
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean1->removeMember("p1");
    EXPECT_TRUE(world->getPropertyId("p1") == 0);

    bean2->removeMember("p1");
    EXPECT_TRUE(world->getPropertyId("p1") == -1);

    bean1->removeMember("p2");
    EXPECT_TRUE(world->getPropertyId("p2") == 1);

    bean2->removeMember("p2");
    EXPECT_TRUE(world->getPropertyId("p2") == -1);

    delete world;
}

TEST(BeanWorld, removeBean)
{
    BeanWorld world;
    Value value;
    oidType oid = 0;
    Bean* bean = world.createBean();
    oid = bean->getId();
    bean->setProperty("p1", 1);
    (*bean)["p2"] = 2;
    world.removeBean(bean->getId());
    EXPECT_TRUE(world.getBean(oid) == nullptr);
}


TEST(BeanWorld, findEqual_basic)
{
    BeanWorld world;
    std::list<Bean*> beans;

    Bean* bean1 = world.createBean();
    bean1->setProperty("double_p", 1.0);
    bean1->setProperty("str_p", "hello");
    bean1->setProperty("bool_p0", false);
    bean1->setProperty("bool_p1", true);
    bean1->setProperty("int_p", 1);
    bean1->setProperty("uint_p", 2U);
    bean1->setProperty("int64_p", 3);
    bean1->setProperty("uint64_p", 4U);


    // world.findEqual("int_p", 1, beans);
    // EXPECT_TRUE(beans.size() == 1);
    // for (auto& bean : beans)
    // {
    //     EXPECT_TRUE((*bean)["int_p"] == 1);
    // }

    // world.findEqual("uint_p", 2U, beans);
    // EXPECT_TRUE(beans.size() == 1);
    // for (auto& bean : beans)
    // {
    //     EXPECT_TRUE((*bean)["uint_p"] == 2U);
    // }

    // world.findEqual("int64_p", 3, beans);
    // EXPECT_TRUE(beans.size() == 1);
    // for (auto& bean : beans)
    // {
    //     EXPECT_TRUE((*bean)["int64_p"] == 3);
    // }

    // world.findEqual("uint64_p", 4U, beans);
    // EXPECT_TRUE(beans.size() == 1);
    // for (auto& bean : beans)
    // {
    //     EXPECT_TRUE((*bean)["uint64_p"] == 4U);
    // }

    Bean* bean2 = world.createBean();
    bean2->setProperty("double_p", 1.0);
    bean2->setProperty("str_p", "hello");
    bean2->setProperty("bool_p0", false);
    bean2->setProperty("bool_p1", true);
    bean2->setProperty("int_p", 1);
    bean2->setProperty("uint_p", 2U);
    bean2->setProperty("int64_p", 3);
    bean2->setProperty("uint64_p", 4U);

    world.findEqual("double_p", 1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["double_p"] == 1.0);
    }

    world.findEqual("bool_p0", false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["bool_p0"] == false);
    }

    world.findEqual("bool_p1", true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["bool_p1"] == true);
    }


    world.findEqual("str_p", "hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["str_p"] == "hello");
    }

   world.findEqual("int_p", 1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["int_p"] == 1);
    }

    world.findEqual("uint_p", 2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["uint_p"] == 2U);
    }

    world.findEqual("int64_p", (Json::Int64)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["int64_p"] == 3);
    }

    world.findEqual("uint64_p", (Json::UInt64)4, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& bean : beans)
    {
        EXPECT_TRUE((*bean)["uint64_p"] == 4U);
    }


}


TEST(BeanWorld, findEqual_type_diff)
{
    BeanWorld world;
    std::list<Bean*> beans;
    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();

    bean1->setProperty("p1", 1);
    bean1->setProperty("p2", 2);
    bean1->setProperty("p3", 3);
    bean1->setProperty("p4", 4);

    bean2->setProperty("p1", 1U);
    bean2->setProperty("p2", 2U);
    bean2->setProperty("p3", 3U);
     bean2->setProperty("p4", 4U);

   world.findEqual("p1", 1, beans);
   EXPECT_TRUE(beans.size() == 1);
   world.findEqual("p1", 1U, beans);
   EXPECT_TRUE(beans.size() == 1);

    world.findEqual("p2", 2, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findEqual("p2", 2U, beans);
    EXPECT_TRUE(beans.size() == 1);

    world.findEqual("p3", 3, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findEqual("p3", 3U, beans);
   EXPECT_TRUE(beans.size() == 1);

    world.findEqual("p4", 4, beans);
    EXPECT_TRUE(beans.size() == 1);
    world.findEqual("p4", 4U, beans);
   EXPECT_TRUE(beans.size() == 1);

}