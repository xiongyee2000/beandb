

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

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

TEST(Bean, Constructor)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);

    Bean *bean = new Bean(0, &world);
    EXPECT_TRUE(nullptr != bean);
}

TEST(Bean, getId)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);

    Bean *bean1 = world.createBean();
    Bean *bean2 = world.createBean();
    EXPECT_TRUE(bean1->getId() >= 0);
    EXPECT_TRUE(bean2->getId() >= 0);
    EXPECT_TRUE(bean1->getId() != bean2->getId());
}

TEST(Bean, isMember)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    TestHelper testHelper;

    init_world(testHelper, world);
    
    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(nullptr));

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(!bean1->isMember(testHelper.p1));
    bean1->setProperty(testHelper.p1, 1);
    EXPECT_TRUE(bean1->isMember(testHelper.p1));
    EXPECT_TRUE(bean1->getMemberNames().size() ==1);

    EXPECT_TRUE(!bean1->isMember(testHelper.p2));
    bean1->setProperty(testHelper.p2, 2);
    EXPECT_TRUE(bean1->isMember(testHelper.p2));
    EXPECT_TRUE(bean1->getMemberNames().size() == 2);

    EXPECT_TRUE(!bean1->isMember(testHelper.p_array_int));
    bean1->createArrayProperty(testHelper.p_array_int);
    EXPECT_TRUE(bean1->isMember(testHelper.p_array_int));
    EXPECT_TRUE(bean1->getMemberNames().size() == 3);

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->isMember(testHelper.r1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 4);

    EXPECT_TRUE(!bean1->isMember(testHelper.r_array_1));
    bean1->createArrayRelation(testHelper.r_array_1);
    EXPECT_TRUE(bean1->isMember(testHelper.r_array_1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 5);
}

TEST(Bean, create_has_remove)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    int err = 0;
    TestHelper testHelper;
    
    init_world(testHelper, world);

    Bean* bean1 = world.createBean();

    err = bean1->createArrayProperty(nullptr);
    EXPECT_TRUE(err == -2);

    EXPECT_TRUE(!bean1->hasProperty(nullptr));
    EXPECT_TRUE(!bean1->hasArrayProperty(nullptr));
    EXPECT_TRUE(!bean1->hasRelation(nullptr));
    EXPECT_TRUE(!bean1->hasArrayRelation(nullptr));

    EXPECT_TRUE(!bean1->hasProperty(testHelper.p1));
    bean1->setProperty(testHelper.p1, 1);
    EXPECT_TRUE(bean1->hasProperty(testHelper.p1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.p1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.p1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.p1));

    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.p_array_int));
    err = bean1->createArrayProperty(testHelper.p_array_int);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.p_array_int));
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.p_array_int));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.p_array_int));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.p_array_int));

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.r1));
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.r1));

    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.r_array_1));
    bean1->createArrayRelation(testHelper.r_array_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.r_array_1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.r_array_1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.r_array_1));
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1));

    value = bean1->removeProperty(nullptr);
    EXPECT_TRUE(value.isNull());

    bean1->setProperty(testHelper.p1, 1);
    value = bean1->removeProperty(testHelper.p1);
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean1->hasProperty(testHelper.p1) == false);

    bean1->createArrayProperty(testHelper.p_array_int);
    bean1->appendProperty(testHelper.p_array_int, 1);
    value = bean1->removeProperty(testHelper.p_array_int);
    EXPECT_TRUE(value[0] == 1);
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.p1) == false);

    auto memberSize = bean1->getMemberNames().size();
    bean1->removeProperty(nullptr);
    EXPECT_TRUE(memberSize == bean1->getMemberNames().size());

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == true);
    bean1->removeProperty(testHelper.r1);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == false);

    bean1->createArrayRelation(testHelper.r_array_1);
    bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1) == true);
    bean1->removeProperty(testHelper.r_array_1);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1) == false);
}

TEST(Bean, property)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    int err = 0;
    TestHelper testHelper;
    Property* property = nullptr;

    init_world(testHelper, world);

    Bean &bean = *world.createBean();

    err = bean.setProperty(testHelper.p1, Json::Value());
    EXPECT_TRUE(err == -1);
    err = bean.setProperty(nullptr, 1);
    EXPECT_TRUE(err == -2);
    err = bean.setProperty(testHelper.p1, 1.0);
    EXPECT_TRUE(err == -3);

    err = bean.setProperty(testHelper.p1, Json::Value::minInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == Json::Value::minInt);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt);

    err = bean.setProperty(testHelper.p1, Json::Value::minInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == Json::Value::minInt64);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == Json::Value::minInt64);

    world.undefineProperty("p1");
    testHelper.p1 = world.defineProperty("p1", Property::UIntType);

    err = bean.setProperty(testHelper.p1, Json::Value::maxUInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == Json::Value::maxUInt);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt);

    err = bean.setProperty(testHelper.p1, Json::Value::maxUInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == Json::Value::maxUInt64);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == Json::Value::maxUInt64);

    world.undefineProperty("p1");
    testHelper.p1 = world.defineProperty("p1", Property::RealType);

    err = bean.setProperty(testHelper.p1, 8.8f);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == 8.8f);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 8.8f);

    err = bean.setProperty(testHelper.p1, 8.8);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == 8.8);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 8.8);

    world.undefineProperty("p1");
    testHelper.p1 = world.defineProperty("p1", Property::StringType);

    bean.setProperty(testHelper.p1, "v1");
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == "v1");
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == "v1");

    bean.setProperty(testHelper.p1, "v1");
    world.undefineProperty("p2");
    testHelper.p2 = world.defineProperty("p2", Property::StringType);
    err = bean.setProperty(testHelper.p2, "v2");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 2);
    EXPECT_TRUE(bean.getProperty(testHelper.p2) == "v2");

    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(value == "v1");
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(testHelper.p1).isNull());

    value = bean.removeProperty(testHelper.p2);
    EXPECT_TRUE(value == "v2");
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.getProperty(testHelper.p2).isNull());

    world.undefineProperty("p1");
    testHelper.p1 = world.defineProperty("p1", Property::BoolType);
    err = bean.setProperty(testHelper.p1, true);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == true);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == true);

    err = bean.setProperty(testHelper.p1, false);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == false);
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == false);

}


TEST(Bean, array_property)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Json::Value value;
    Property* property;
    int err = 0;
    TestHelper testHelper;

    init_world(testHelper, world);

    Bean &bean = *world.createBean();
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.p_array_int) == false);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 0);
    Value value2 = bean.getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value2.isNull());

    err = bean.appendProperty(testHelper.p_array_int, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.appendProperty(nullptr, 0);
    EXPECT_TRUE(err == -2);

    err = bean.setProperty(testHelper.p_array_int, 0, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.setProperty(nullptr, 0, 1);
    EXPECT_TRUE(err == -2);

    property = world.defineArrayProperty("p_array_int", Property::IntType);
    EXPECT_TRUE(property == world.getProperty("p_array_int"));
    err = bean.appendProperty(testHelper.p_array_int, 1);
    EXPECT_TRUE(err == -4);
    err = bean.setProperty(testHelper.p_array_int, 0, 1);
    EXPECT_TRUE(err == -4);

    //create empty array
    err = bean.createArrayProperty(testHelper.p_array_int);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.p_array_int) == true);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 0);
    value = bean.getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value.isNull());

    //append
    err = bean.appendProperty(testHelper.p_array_int, "0");
    EXPECT_TRUE(err == -3);
    err = bean.appendProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 1);
    value = bean.getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 0);
    value = bean.getArrayProperty(testHelper.p_array_int, 1);
    EXPECT_TRUE(value.isNull());
    err = bean.appendProperty(testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 2);
    value = bean.getArrayProperty(testHelper.p_array_int, 1);
    EXPECT_TRUE(value == 1);

    //setProperty()
    err = bean.setProperty(testHelper.p_array_int, 99, 99);
    EXPECT_TRUE(err == -5);
    err = bean.setProperty(testHelper.p_array_int, 0, 99);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 2);
    value = bean.getArrayProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    err = bean.setProperty(testHelper.p_array_int, 1, 999);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 2);
    value = bean.getArrayProperty(testHelper.p_array_int, 1);
    EXPECT_TRUE(value == 999);

    //removeProperty()
    value = bean.removeProperty(nullptr, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    bean.setProperty(testHelper.p1, 1);
    value = bean.removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.p_array_int, 3);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 1);
    value = bean.removeProperty(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 999);
    EXPECT_TRUE(bean.getArraySize(testHelper.p_array_int) == 0);
}

TEST(Bean, relation)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    int err = 0;
    TestHelper testHelper;

    init_world(testHelper, world);

    Bean* bean1 = world.createBean();

    EXPECT_TRUE(!bean1->getRelationBeanId(nullptr));

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p1));
    bean1->setProperty(testHelper.p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p1));

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p_array_int));
    bean1->createArrayProperty(testHelper.p_array_int);
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p_array_int));

    Bean* bean2 = world.createBean();

    err = bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(testHelper.p1, bean2);
    EXPECT_TRUE(err == -3);

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.r1));
    err = bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(testHelper.r1) == bean2->getId());

}

TEST(Bean, array_relation)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    int err = 0;
    TestHelper testHelper;
    
    init_world(testHelper, world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    err = bean1->appendRelation(testHelper.r_array_1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->appendRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(testHelper.p1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(err == -4);

    err = bean1->setRelation(testHelper.r_array_1, 0, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, 0, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(testHelper.p1, 0, bean2);
    EXPECT_TRUE(err == -3);
    err = bean1->setRelation(testHelper.r_array_1, 0, bean2);
    EXPECT_TRUE(err == -4);

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.r_array_1, 0));
    err = bean1->createArrayRelation(nullptr);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(testHelper.p1);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(testHelper.r_array_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(testHelper.r_array_1) == 0);

    err = bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(testHelper.r_array_1) == 1);

    err = bean1->setRelation(testHelper.r_array_1, 1, bean2);
    EXPECT_TRUE(err == -5);
    err = bean1->setRelation(testHelper.r_array_1, 0, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(testHelper.r_array_1, 0) == bean2->getId());
    err = bean1->setRelation(testHelper.r_array_1, 0, bean3);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(testHelper.r_array_1, 0) == bean3->getId());
    bean1->setProperty(testHelper.p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p1, 0));
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.r_array_1, 1));

    //getArraySize()
    EXPECT_TRUE(bean1->getArraySize(nullptr) == 0);
    EXPECT_TRUE(bean1->getArraySize(testHelper.p1) == 0);

    //removeProperty()
    value = bean3->removeProperty(nullptr, 0);
    EXPECT_TRUE(value.isNull());
    value = bean3->removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    bean3->setProperty(testHelper.p1, 1);
    value = bean3->removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    value = bean3->removeProperty(testHelper.p_array_int, 3);
    EXPECT_TRUE(value.isNull());

    err = bean3->createArrayRelation(testHelper.r_array_1);
    err = bean3->appendRelation(testHelper.r_array_1, bean1);
    err = bean3->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(bean3->getArraySize(testHelper.r_array_1) == 2);

    value = bean3->removeProperty(testHelper.r_array_1, 0);
    EXPECT_TRUE(value == bean1->getId());
    EXPECT_TRUE(bean3->getArraySize(testHelper.r_array_1) == 1);
    value = bean3->removeProperty(testHelper.r_array_1, 0);
    EXPECT_TRUE(value == bean2->getId());
    EXPECT_TRUE(bean3->getArraySize(testHelper.r_array_1) == 0);
}

TEST(Bean, nativeData)
{
    DummyBeanDB dummyDB;
    BeanWorld world = *dummyDB.getWorld();
    Value value;
    int err = 0;
    TestHelper testHelper;
    Property* property = nullptr;

    initTestHelper(testHelper, dummyDB);

    Bean* bean = world.createBean();

    setBeanNativeData(testHelper, bean);
    validateBeanNativeData(testHelper, bean);
}


TEST(Bean, removeAllProperties)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld& world = *testdb.getWorld();

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.reInit();
    testdb.connect();

    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.createBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);

    bean2 = testdb.createBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);

    bean3 = testdb.createBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->createArrayRelation(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = testdb.saveBean(bean1);
    err = testdb.saveBean(bean2);
    err = testdb.saveBean(bean3);

    EXPECT_TRUE(bean1->getMemberNames().size() > 0);
    EXPECT_TRUE(bean2->getMemberNames().size() > 0);
    EXPECT_TRUE(bean3->getMemberNames().size() > 0);

    bean1->removeAllProperties();
    bean2->removeAllProperties();
    bean3->removeAllProperties();

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(bean2->getMemberNames().size() == 0);
    EXPECT_TRUE(bean3->getMemberNames().size() == 0);

    testdb.reInit();
    testdb.disconnect();
}


TEST(Bean, removeNativeData)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld& world = *testdb.getWorld();

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.reInit();
    testdb.connect();

    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.createBean();
    beanId_1 = bean1->getId();
    setBeanNativeData(testHelper, bean1);

    EXPECT_TRUE(bean1->getNativeData().getMemberNames().size() > 0);

    bean1 = testdb.getBean(beanId_1);
    err = bean1->removeNativeData();
    EXPECT_TRUE(err == 0);

    EXPECT_TRUE(bean1->getNativeData().getMemberNames().size() == 0);

    testdb.reInit();
    testdb.disconnect();
}


TEST(Bean, clear)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld& world = *testdb.getWorld();

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.reInit();
    testdb.connect();

    initTestHelper(testHelper, testdb, false);

    bean1 = testdb.createBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);
    setBeanNativeData(testHelper, bean1);

    bean2 = testdb.createBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);
    setBeanNativeData(testHelper, bean2);

    bean3 = testdb.createBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);
    setBeanNativeData(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->createArrayRelation(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = testdb.saveBean(bean1);
    err = testdb.saveBean(bean2);
    err = testdb.saveBean(bean3);

    EXPECT_TRUE(bean1->getMemberNames().size() > 0);
    EXPECT_TRUE(bean2->getMemberNames().size() > 0);
    EXPECT_TRUE(bean3->getMemberNames().size() > 0);
    EXPECT_TRUE(bean1->getNativeData().getMemberNames().size() > 0);

    bean1->clear();
    bean2->clear();
    bean3->clear();

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(bean2->getMemberNames().size() == 0);
    EXPECT_TRUE(bean3->getMemberNames().size() == 0);
    EXPECT_TRUE(bean1->getNativeData().getMemberNames().size() == 0);

    testdb.reInit();
    testdb.disconnect();
}


