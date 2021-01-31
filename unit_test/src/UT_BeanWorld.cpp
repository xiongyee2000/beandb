

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

TEST(BeanWorld, findBean)
{
    BeanWorld *world = new BeanWorld();
    Bean* bean_ = nullptr;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    bean_ = world->findBean(bean1->getId());
    EXPECT_TRUE(bean_ == bean1);
    bean_ = world->findBean(bean2->getId());
    EXPECT_TRUE(bean_ == bean2);
    bean_ = world->findBean(bean3->getId());
    EXPECT_TRUE(bean_ == bean3);

    bean_ = world->findBean(555);
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

TEST(BeanWorld, getProperties)
{
    BeanWorld *world = new BeanWorld();
    Value value;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    bean1->setProperty(nullptr, 1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    bean1->setProperty("", 1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    bean1->setProperty(string("").c_str(), 1);
    EXPECT_TRUE(world->getProperties().size() == 0);

    bean1->setProperty("p1", 8);
    EXPECT_TRUE(world->getProperties().size() == 1);

    bean1->setProperty("p2", 8);
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean2->setProperty("p1", "v1");
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean2->setProperty("p2", "v2");
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean1->removeProperty("p1");
    EXPECT_TRUE(world->getProperties().size() == 2);

    bean2->removeProperty("p1");
    EXPECT_TRUE(world->getProperties().size() == 1);

    bean1->removeProperty("p2");
    EXPECT_TRUE(world->getProperties().size() == 1);

    bean2->removeProperty("p2");
    EXPECT_TRUE(world->getProperties().size() == 0);

    delete world;
}

TEST(BeanWorld, getPropertyIndex)
{
    BeanWorld *world = new BeanWorld();
    Value value;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();

    bean1->setProperty("p1", 8);
    EXPECT_TRUE(world->getPropertyIndex("p1") == 0);

    bean1->setProperty("p2", 8);
    EXPECT_TRUE(world->getPropertyIndex("p2") == 1);

    bean2->setProperty("p1", "v1");
    EXPECT_TRUE(world->getPropertyIndex("p1") == 0);

    bean2->setProperty("p2", "v2");
    EXPECT_TRUE(world->getPropertyIndex("p2") == 1);

    bean1->removeProperty("p1");
    EXPECT_TRUE(world->getPropertyIndex("p1") == 0);

    bean2->removeProperty("p1");
    EXPECT_TRUE(world->getPropertyIndex("p1") == -1);

    bean1->removeProperty("p2");
    EXPECT_TRUE(world->getPropertyIndex("p2") == 1);

    bean2->removeProperty("p2");
    EXPECT_TRUE(world->getPropertyIndex("p2") == -1);

    delete world;
}
