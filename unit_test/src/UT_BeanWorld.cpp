

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/beandb/BeanWorld.h"

#include "./common.h"
#include "./DummyBeanDB.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::beandb;

void basic()
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();

    EXPECT_TRUE(nullptr != world);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);
    EXPECT_TRUE(world->getProperties().size() == 0);

    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

    Bean *bean1 = world->newBean();
    EXPECT_TRUE(nullptr != bean1);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 1);

    Bean *bean2 = world->newBean();
    EXPECT_TRUE(nullptr != bean2);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 2);

    Bean *bean3 = world->newBean();
    EXPECT_TRUE(nullptr != bean3);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 3);

    world->unloadBean(bean1);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 2);

    world->unloadBean(bean2);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 1);

    world->unloadBean(bean3);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

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
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    
    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();
    Bean *bean3 = world->newBean();
    auto& beans  = world->getCachedBeans();
    EXPECT_TRUE(beans.size() == 3);
}

TEST(BeanWorld, getBean)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    Bean* bean = nullptr;

    std::vector<int>::const_iterator it;

    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();
    Bean *bean3 = world->newBean();
    bean = world->getBean(bean1->getId());
    EXPECT_TRUE(bean == bean1);
    bean = world->getBean(bean2->getId());
    EXPECT_TRUE(bean == bean2);
    bean = world->getBean(bean3->getId());
    EXPECT_TRUE(bean == bean3);

    bean = world->getBean(555);
    EXPECT_TRUE(bean == nullptr);

}

TEST(BeanWorld, clear)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();

    world->newBean();
    world->newBean();
    world->newBean();
    EXPECT_TRUE(world->getCachedNumOfBeans() == 3);

    world->unloadAll();
    EXPECT_TRUE(world->getCachedNumOfBeans() == 0);

}

TEST(BeanWorld, defineProperty_undefineProperty)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    Value value;
    Property* property = nullptr;

    property = world->defineProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world->defineArrayProperty(nullptr, Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayProperty("", Property::IntType);
    EXPECT_TRUE(property == nullptr);

    property = world->defineRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world->defineRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world->defineArrayRelation(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayRelation("");
    EXPECT_TRUE(property == nullptr);

    property = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world->undefineProperty(world->getProperty("p1"));
    EXPECT_TRUE(nullptr == world->getProperty("p1"));

    property = world->defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineArrayProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world->undefineProperty(world->getProperty("p1"));
    EXPECT_TRUE(nullptr == world->getProperty("p1"));

    property = world->defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world->undefineProperty(world->getProperty("p1"));
    EXPECT_TRUE(nullptr == world->getProperty("p1"));

    property = world->defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineArrayRelation("p1");
    EXPECT_TRUE(property != nullptr && property == world->getProperty("p1"));
    property = world->defineProperty("p1", Property::UIntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineArrayProperty("p1", Property::IntType);
    EXPECT_TRUE(property == nullptr);
    property = world->defineRelation("p1");
    EXPECT_TRUE(property == nullptr);
    world->undefineProperty(world->getProperty("p1"));
    EXPECT_TRUE(nullptr == world->getProperty("p1"));
}

TEST(BeanWorld, getProperty)
{
    DummyBeanDB dummyDB;
    BeanWorld* world;
    Value value;
    Property* property = nullptr;
    TestHelper testHelper;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world);

    property = world->getProperty(nullptr);
    EXPECT_TRUE(property == nullptr);
    property = world->getProperty("");
    EXPECT_TRUE(property == nullptr);
    property = world->getProperty("a");
    EXPECT_TRUE(property == nullptr);

    property = world->defineProperty("p1", Property::IntType);
    property = world->getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");

    property = world->getProperty("p1");
    EXPECT_TRUE(property->getName() == "p1");

    property = world->getProperty(testHelper.p1->getId());
    EXPECT_TRUE(property->getName() == "p1");

    property = world->getProperty(999);
    EXPECT_TRUE(property == nullptr);
}

TEST(BeanWorld, getProperties)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    Value value;
    Property *p1, *p2;

    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();

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

    bean1->set(p1, 1);
    bean2->set(p1, 2);

    world->undefineProperty(p1);
    EXPECT_TRUE(world->getProperties().size() == 1);
    EXPECT_TRUE(bean1->hasPrimaryProperty(p1) == false);
    world->undefineProperty(p2);
    EXPECT_TRUE(world->getProperties().size() == 0);
    EXPECT_TRUE(bean2->hasPrimaryProperty(p1) == false);
}


TEST(BeanWorld, unloadBean)
{
    int err = 0;
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    Value value;
    oidType oid = 0;
    TestHelper testHelper;

    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    Bean* bean4 = world->newBean();

    oid = bean1->getId();
    world->unloadBean(bean1);
    EXPECT_TRUE(world->getBean(oid, false) == nullptr);
    bean1 = world->newBean();

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == bean2->getId());
    world->unloadBean(bean2);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == 2);
    bean2 = world->newBean();

    bean1->setRelation(testHelper.r1, bean3);
    bean1->setRelation(testHelper.r2, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r2, bean3);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean1->getRelation(testHelper.r2) == bean3->getId());
    EXPECT_TRUE(bean2->getRelation(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean2->getRelation(testHelper.r2) == bean3->getId());
    EXPECT_TRUE(world->getCachedNumOfBeans() == 4);
    oid = bean3->getId();
    world->unloadBean(bean3);
    bean3 = nullptr;
    EXPECT_TRUE(world->getBean(oid, false) == nullptr);
    EXPECT_TRUE(world->getCachedNumOfBeans() == 3);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == oid);
    EXPECT_TRUE(bean1->getRelation(testHelper.r2) == oid);
    EXPECT_TRUE(bean2->getRelation(testHelper.r1) == oid);
    EXPECT_TRUE(bean2->getRelation(testHelper.r2) == oid);
    bean3 = world->newBean();
    oid = bean3->getId();

    //array
    bean1->addArray(testHelper.r_array_1);
    bean1->addArray(testHelper.r_array_2);
    bean1->appendRelation(testHelper.r_array_1, bean3);
    bean1->appendRelation(testHelper.r_array_1, bean4);
    bean1->appendRelation(testHelper.r_array_2, bean3);
    bean1->appendRelation(testHelper.r_array_2, bean4);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 1) == bean4->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 1) == bean4->getId());
    world->unloadBean(bean3);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == oid);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 1) == bean4->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == oid);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 1) == bean4->getId());
    bean3 = world->newBean();

    // bean2->addArray(testHelper.r_array_1);
    // bean2->addArray(testHelper.r_array_2);    
    // bean2->appendRelation(testHelper.r_array_1, bean4);
    // bean2->appendRelation(testHelper.r_array_2, bean4);
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean4->getId());
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == bean4->getId());
    // EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == bean4->getId());
    // EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_2, 0) == bean4->getId());
    // world->deleteBean(bean4);
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == 0);
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == 0);
    // EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == 0);
    // EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_2, 0) == 0);
    // bean4 = world->newBean();

    // bean1->appendRelation(testHelper.r_array_1, bean2);
    // bean2->appendRelation(testHelper.r_array_1, bean3);
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean2->getId());
    // EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == bean3->getId());
    // EXPECT_TRUE(bean3->m_subjectMap_.size() == 1);
    // world->deleteBean(bean2);
    // EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == 0);
    // EXPECT_TRUE(bean3->m_subjectMap_.size() == 0);

}


TEST(BeanWorld, deleteBean)
{
    int err = 0;
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    Value value;
    oidType oid = 0;
    TestHelper testHelper;

    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    Bean* bean4 = world->newBean();

    oid = bean1->getId();
    world->deleteBean(bean1);
    EXPECT_TRUE(world->getBean(oid, false) == nullptr);
    bean1 = world->newBean();

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == bean2->getId());
    world->deleteBean(bean2);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == 0);
    bean2 = world->newBean();

    bean1->setRelation(testHelper.r1, bean3);
    bean1->setRelation(testHelper.r2, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r2, bean3);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean1->getRelation(testHelper.r2) == bean3->getId());
    EXPECT_TRUE(bean2->getRelation(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean2->getRelation(testHelper.r2) == bean3->getId());
    EXPECT_TRUE(world->getCachedNumOfBeans() == 4);
    world->deleteBean(bean3);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == 0);
    EXPECT_TRUE(bean1->getRelation(testHelper.r2) == 0);
    EXPECT_TRUE(bean2->getRelation(testHelper.r1) == 0);
    EXPECT_TRUE(bean2->getRelation(testHelper.r2) == 0);
    bean3 = world->newBean();

    bean1->setRelation(testHelper.r1, bean2);
    bean2->setRelation(testHelper.r1, bean3);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == bean2->getId());
    EXPECT_TRUE(bean2->getRelation(testHelper.r1) == bean3->getId());
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 1);
    world->deleteBean(bean2);
    EXPECT_TRUE(bean1->getRelation(testHelper.r1) == 0);
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 0);
    bean2 = world->newBean();

    //array
    bean1->addArray(testHelper.r_array_1);
    bean1->addArray(testHelper.r_array_2);
    bean1->appendRelation(testHelper.r_array_1, bean3);
    bean1->appendRelation(testHelper.r_array_1, bean4);
    bean1->appendRelation(testHelper.r_array_2, bean3);
    bean1->appendRelation(testHelper.r_array_2, bean4);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 1) == bean4->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == bean3->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 1) == bean4->getId());
    world->deleteBean(bean3);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == bean4->getId());
    bean3 = world->newBean();

    bean2->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_2);    
    bean2->appendRelation(testHelper.r_array_1, bean4);
    bean2->appendRelation(testHelper.r_array_2, bean4);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == bean4->getId());
    EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == bean4->getId());
    EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_2, 0) == bean4->getId());
    world->deleteBean(bean4);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == 0);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_2, 0) == 0);
    EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == 0);
    EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_2, 0) == 0);
    bean4 = world->newBean();

    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean3);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == bean2->getId());
    EXPECT_TRUE(bean2->getReationAt(testHelper.r_array_1, 0) == bean3->getId());
    EXPECT_TRUE(bean3->m_subjectMap_.size() == 1);
    world->deleteBean(bean2);
    EXPECT_TRUE(bean1->getReationAt(testHelper.r_array_1, 0) == 0);
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

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    bean2 = world->newBean();
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