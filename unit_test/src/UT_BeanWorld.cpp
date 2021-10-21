

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
#include "./DummyBeanDB.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

void basic()
{
    DummyBeanDB dummyDB;
    BeanWorld *world = new BeanWorld((AbstractBeanDB&)dummyDB);
    EXPECT_TRUE(nullptr != world);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);
    EXPECT_TRUE(world->getProperties().size() == 0);

    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

    Bean *bean1 = world->createBean();
    EXPECT_TRUE(nullptr != bean1);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 1);

    Bean *bean2 = world->createBean();
    EXPECT_TRUE(nullptr != bean2);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 2);

    Bean *bean3 = world->createBean();
    EXPECT_TRUE(nullptr != bean3);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 3);

    world->unloadBean(bean1->getId());
    EXPECT_TRUE(world->getCachedNumOfBeans() == 2);

    world->unloadBean(bean2->getId());
    EXPECT_TRUE(world->getCachedNumOfBeans() == 1);

    world->unloadBean(bean3->getId());
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

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
    DummyBeanDB dummyDB;
    BeanWorld *world = new BeanWorld((AbstractBeanDB&)dummyDB);
    
    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    auto& beans  = world->getCachedBeans();
    EXPECT_TRUE(beans.size() == 3);

    delete world;
}

TEST(BeanWorld, getBean)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = new BeanWorld((AbstractBeanDB&)dummyDB);
    Bean* bean = nullptr;

    std::vector<int>::const_iterator it;

    Bean *bean1 = world->createBean();
    Bean *bean2 = world->createBean();
    Bean *bean3 = world->createBean();
    bean = world->getBean(bean1->getId());
    EXPECT_TRUE(bean == bean1);
    bean = world->getBean(bean2->getId());
    EXPECT_TRUE(bean == bean2);
    bean = world->getBean(bean3->getId());
    EXPECT_TRUE(bean == bean3);

    bean = world->getBean(555);
    EXPECT_TRUE(bean == nullptr);

    delete world;
}

TEST(BeanWorld, clear)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = new BeanWorld((AbstractBeanDB&)dummyDB);

    world->createBean();
    world->createBean();
    world->createBean();
    EXPECT_TRUE(world->getCachedNumOfBeans() == 3);

    world->clear();
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

    delete world;
}

TEST(BeanWorld, defineProperty_undefineProperty)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    Property* property = nullptr;

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
    property = world.defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty(world.getProperty("p1"));
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty(world.getProperty("p1"));
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty(world.getProperty("p1"));
    EXPECT_TRUE(nullptr == world.getProperty("p1"));

    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world.getProperty("p1"));
    property = world.defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world.defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world.undefineProperty(world.getProperty("p1"));
    EXPECT_TRUE(nullptr == world.getProperty("p1"));
}

TEST(BeanWorld, getProperty)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    const Property* property = nullptr;
    TestHelper testHelper;

    initTestHelper(testHelper, world);

    property = world.getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world.getProperty("a");
    EXPECT_TRUE(property == nullptr);

    property = world.defineProperty("p1", Property::IntType);
    property = world.getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");

    property = ((BeanWorld*)&world)->getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");
}

TEST(BeanWorld, getProperties)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = new BeanWorld((AbstractBeanDB&)dummyDB);
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

    world->undefineProperty(p1);
    EXPECT_TRUE(world->getProperties().size() == 1);
    EXPECT_TRUE(bean1->hasProperty(p1) == false);
    world->undefineProperty(p2);
    EXPECT_TRUE(world->getProperties().size() == 0);
    EXPECT_TRUE(bean2->hasProperty(p1) == false);
    delete world;
}

TEST(BeanWorld, removeBean)
{
    int err = 0;
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    oidType oid = 0;
    TestHelper testHelper;

    initTestHelper(testHelper, world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();
    Bean* bean4 = world.createBean();

    oid = bean1->getId();
    world.unloadBean(bean1->getId());
    EXPECT_TRUE(world.getBean(oid, false) == nullptr);
    bean1 = world.createBean();

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == bean2->getId());
    world.unloadBean(bean2->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == 0);
    bean2 = world.createBean();

    bean1->setRelation(testHelper.r1, bean3);
    bean1->setRelation(testHelper.r2, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r2, bean3);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r2) == bean3->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r2) == bean3->getId());
    world.unloadBean(bean3->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == 0);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r2) == 0);
    EXPECT_TRUE(bean2->getObjectId(testHelper.r1) == 0);
    EXPECT_TRUE(bean2->getObjectId(testHelper.r2) == 0);
    bean3 = world.createBean();

    bean1->setRelation(testHelper.r1, bean2);
    bean2->setRelation(testHelper.r1, bean3);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == bean2->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 1);
    world.unloadBean(bean2->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == 0);
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 0);
    bean2 = world.createBean();

    //array
    bean1->createArrayRelation(testHelper.r_array_1);
    bean1->createArrayRelation(testHelper.r_array_2);
    bean1->appendRelation(testHelper.r_array_1, bean3);
    bean1->appendRelation(testHelper.r_array_1, bean4);
    bean1->appendRelation(testHelper.r_array_2, bean3);
    bean1->appendRelation(testHelper.r_array_2, bean4);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 1) == bean4->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_2, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_2, 1) == bean4->getId());
    world.unloadBean(bean3->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_2, 0) == bean4->getId());
    bean3 = world.createBean();

    bean2->createArrayRelation(testHelper.r_array_1);
    bean2->createArrayRelation(testHelper.r_array_2);    
    bean2->appendRelation(testHelper.r_array_1, bean4);
    bean2->appendRelation(testHelper.r_array_2, bean4);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_2, 0) == bean4->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r_array_2, 0) == bean4->getId());
    world.unloadBean(bean4->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == 0);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_2, 0) == 0);
    EXPECT_TRUE(bean2->getObjectId(testHelper.r_array_1, 0) == 0);
    EXPECT_TRUE(bean2->getObjectId(testHelper.r_array_2, 0) == 0);
    bean4 = world.createBean();

    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean3);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean2->getId());
    EXPECT_TRUE(bean2->getObjectId(testHelper.r_array_1, 0) == bean3->getId());
        EXPECT_TRUE(bean3->m_subjectMap_.size() == 1);
    world.unloadBean(bean2->getId());
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == 0);
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 0);

}


TEST(BeanWorld, saveAll)
{
    int err = 0;
    DummyBeanDB dummyDB;
    BeanWorld* world = nullptr;
    TestHelper testHelper;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world);

    bean1 = world->createBean();
    beanId_1 = bean1->getId();
    bean2 = world->createBean();
    beanId_2 = bean2->getId();

    setBeanProperties(testHelper, bean1);
    setBeanProperties(testHelper, bean2);
    setBeanNativeData(testHelper, bean1);
    setBeanNativeData(testHelper, bean2);

    err = world->saveAll();
    EXPECT_TRUE(err == 0);

    dummyDB.disconnect();
    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world);
    
    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);

    validateBean(testHelper, bean1);
    validateBean(testHelper, bean2);
}