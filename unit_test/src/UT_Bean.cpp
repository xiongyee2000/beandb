

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
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    Bean *bean = new Bean(0, world);
    EXPECT_TRUE(nullptr != bean);
}

TEST(Bean, getId)
{
    DummyBeanDB dummyDB;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();
    EXPECT_TRUE(bean1->getId() >= 0);
    EXPECT_TRUE(bean2->getId() >= 0);
    EXPECT_TRUE(bean1->getId() != bean2->getId());
}


TEST(Bean, create_has_remove)
{
    DummyBeanDB dummyDB;
    Value value;
    int err = 0;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();

    err = bean1->addArray(nullptr);
    EXPECT_TRUE(err == -2);

    EXPECT_TRUE(!bean1->hasPrimaryProperty(nullptr));
    EXPECT_TRUE(!bean1->hasArrayProperty(nullptr));
    EXPECT_TRUE(!bean1->hasRelation(nullptr));
    EXPECT_TRUE(!bean1->hasArrayRelation(nullptr));

    EXPECT_TRUE(!bean1->hasPrimaryProperty(testHelper.p1));
    bean1->set(testHelper.p1, 1);
    EXPECT_TRUE(bean1->hasPrimaryProperty(testHelper.p1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.p1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.p1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.p1));

    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.p_array_int));
    err = bean1->addArray(testHelper.p_array_int);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasPrimaryProperty(testHelper.p_array_int));
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.p_array_int));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.p_array_int));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.p_array_int));

    Bean* bean2 = world->newBean();

    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(!bean1->hasPrimaryProperty(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.r1));
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.r1));

    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.r_array_1));
    bean1->addArray(testHelper.r_array_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasPrimaryProperty(testHelper.r_array_1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.r_array_1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.r_array_1));
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1));

    value = bean1->remove(nullptr);
    EXPECT_TRUE(value != 0);

    bean1->set(testHelper.p1, 1);
    value = bean1->remove(testHelper.p1);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean1->hasPrimaryProperty(testHelper.p1) == false);

    bean1->addArray(testHelper.p_array_int);
    bean1->append(testHelper.p_array_int, 1);
    value = bean1->remove(testHelper.p_array_int);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.p1) == false);

    auto memberSize = bean1->getMemberNames().size();
    bean1->remove(nullptr);
    EXPECT_TRUE(memberSize == bean1->getMemberNames().size());

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == true);
    bean1->remove(testHelper.r1);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == false);

    bean1->addArray(testHelper.r_array_1);
    bean1->append(testHelper.r_array_1, bean2);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1) == true);
    bean1->remove(testHelper.r_array_1);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.r_array_1) == false);
}

TEST(Bean, property)
{
    DummyBeanDB dummyDB;
    Value value;
    int err = 0;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);

    Bean &bean = *world->newBean();

    err = bean.set(testHelper.p1, Json::Value());
    EXPECT_TRUE(err == -1);
    err = bean.set(nullptr, 1);
    EXPECT_TRUE(err == -2);
    err = bean.set(testHelper.p1, 1.0);
    EXPECT_TRUE(err == -3);

    err = bean.set(testHelper.p1, 1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == 1);
    err = bean.set(testHelper.p1, Json::Value::minInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == Json::Value::minInt);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    err = bean.set(testHelper.p1, (Json::Int64)1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == (Json::Int64)1);
    err = bean.set(testHelper.p1, Json::Value::minInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == Json::Value::minInt64);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    world->undefineProperty(testHelper.p1);
    testHelper.p1 = world->defineProperty("p1", Property::UIntType);

    err = bean.set(testHelper.p1, (Json::UInt)1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) ==  (Json::UInt)1);
    err = bean.set(testHelper.p1, Json::Value::maxUInt);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == Json::Value::maxUInt);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    err = bean.set(testHelper.p1, (Json::Value::UInt64)1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == (Json::Value::UInt64)1);
    err = bean.set(testHelper.p1, Json::Value::maxUInt64);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == Json::Value::maxUInt64);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    world->undefineProperty(testHelper.p1);
    testHelper.p1 = world->defineProperty("p1", Property::RealType);

    err = bean.set(testHelper.p1, 1.1f);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == 1.1f);
    err = bean.set(testHelper.p1, 8.8f);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == 8.8f);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value ==0);

    err = bean.set(testHelper.p1, 8.8);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == 8.8);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    world->undefineProperty(testHelper.p1);
    testHelper.p1 = world->defineProperty("p1", Property::StringType);


    err = bean.set(testHelper.p1, "xxx");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == "xxx");
    err = bean.set(testHelper.p1, "str1");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == "str1");
    err = bean.set(testHelper.p1, std::string("str2"));
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.get(testHelper.p1) == "str2");
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    bean.set(testHelper.p1, "v1");
    world->undefineProperty(testHelper.p2);
    testHelper.p2 = world->defineProperty("p2", Property::StringType);
    err = bean.set(testHelper.p2, "v2");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 2);
    EXPECT_TRUE(bean.get(testHelper.p2) == "v2");

    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.get(testHelper.p1).isNull());

    value = bean.remove(testHelper.p2);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.get(testHelper.p2).isNull());

    world->undefineProperty(testHelper.p1);
    testHelper.p1 = world->defineProperty("p1", Property::BoolType);
    err = bean.set(testHelper.p1, true);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.get(testHelper.p1) == true);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

    err = bean.set(testHelper.p1, false);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getMemberNames().size() == 1);
    EXPECT_TRUE(bean.get(testHelper.p1) == false);
    value = bean.remove(testHelper.p1);
    EXPECT_TRUE(bean.getMemberNames().size() == 0);
    EXPECT_TRUE(bean.hasPrimaryProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == 0);

}


TEST(Bean, array_property)
{
    DummyBeanDB dummyDB;
    Json::Value value;
    Property* property;
    int err = 0;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);

    Bean &bean = *world->newBean();
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.p_array_int) == false);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 0);
    Value value2 = bean.getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value2.isNull());

    err = bean.append(testHelper.p_array_int, Value::nullRef);
    EXPECT_TRUE(err == -1);
    err = bean.append(nullptr, 0);
    EXPECT_TRUE(err == -2);

    err = bean.setAt(testHelper.p_array_int, 0, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.setAt(nullptr, 0, 1);
    EXPECT_TRUE(err == -2);

    property = world->defineArrayProperty("p_array_int", Property::IntType);
    EXPECT_TRUE(property == world->getProperty("p_array_int"));
    err = bean.append(testHelper.p_array_int, 1);
    EXPECT_TRUE(err == -4);
    err = bean.setAt(testHelper.p_array_int, 0, 1);
    EXPECT_TRUE(err == -4);

    //create empty array
    err = bean.addArray(testHelper.p_array_int);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.p_array_int) == true);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 0);
    value = bean.getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value.isNull());
    err = bean.addArray(testHelper.p_array_str);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.p_array_str) == true);
    EXPECT_TRUE(bean.size(testHelper.p_array_str) == 0);
    value = bean.getAt(testHelper.p_array_str, 0);
    EXPECT_TRUE(value.isNull());

    //append
    err = bean.append(testHelper.p_array_int, "0");
    EXPECT_TRUE(err == -3);
    err = bean.append(testHelper.p_array_int, 0);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 1);
    value = bean.getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 0);
    value = bean.getAt(testHelper.p_array_int, 1);
    EXPECT_TRUE(value.isNull());
    err = bean.append(testHelper.p_array_int, 1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 2);
    value = bean.getAt(testHelper.p_array_int, 1);
    EXPECT_TRUE(value == 1);

    err = bean.append(testHelper.p_array_str, 0);
    EXPECT_TRUE(err == -3);
    err = bean.append(testHelper.p_array_str, "0");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_str) == 1);
    value = bean.getAt(testHelper.p_array_str, 0);
    EXPECT_TRUE(value == "0");
    value = bean.getAt(testHelper.p_array_str, 1);
    EXPECT_TRUE(value.isNull());
    err = bean.append(testHelper.p_array_str, "1");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_str) == 2);
    value = bean.getAt(testHelper.p_array_str, 1);
    EXPECT_TRUE(value == "1");

    //setAt()
    err = bean.setAt(testHelper.p_array_int, 99, 99);
    EXPECT_TRUE(err == -5);
    err = bean.setAt(testHelper.p_array_int, 0, 99);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 2);
    value = bean.getAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 99);
    err = bean.setAt(testHelper.p_array_int, 1, 999);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 2);
    value = bean.getAt(testHelper.p_array_int, 1);
    EXPECT_TRUE(value == 999);

    err = bean.setAt(testHelper.p_array_str, 99, "99");
    EXPECT_TRUE(err == -5);
    err = bean.setAt(testHelper.p_array_str, 0, "0");
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_str) == 2);
    value = bean.getAt(testHelper.p_array_str, 0);
    EXPECT_TRUE(value == "0");
    err = bean.setAt(testHelper.p_array_str, 1, std::string("1"));
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_str) == 2);
    value = bean.getAt(testHelper.p_array_str, 1);
    EXPECT_TRUE(value == "1");

    //removeAt()
    value = bean.removeAt(nullptr, 0);
    EXPECT_TRUE(value != 0);
    value = bean.removeAt(testHelper.p1, 0);
    EXPECT_TRUE(value != 0);
    bean.set(testHelper.p1, 1);
    value = bean.removeAt(testHelper.p1, 0);
    EXPECT_TRUE(value != 0);
    value = bean.removeAt(testHelper.p_array_int, 3);
    EXPECT_TRUE(value != 0);
    value = bean.removeAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 1);
    value = bean.removeAt(testHelper.p_array_int, 0);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 0);
    value = bean.removeAt(testHelper.p_array_str, 0);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean.size(testHelper.p_array_int) == 0);
}

TEST(Bean, relation)
{
    DummyBeanDB dummyDB;
    Value value;
    int err = 0;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    EXPECT_TRUE(!bean1->getObjectId(nullptr));

    EXPECT_TRUE(!bean1->getObjectId(testHelper.p1));
    bean1->set(testHelper.p1, 1);
    EXPECT_TRUE(!bean1->getObjectId(testHelper.p1));

    EXPECT_TRUE(!bean1->getObjectId(testHelper.p_array_int));
    bean1->addArray(testHelper.p_array_int);
    EXPECT_TRUE(!bean1->getObjectId(testHelper.p_array_int));


    err = bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(testHelper.p1, bean2);
    EXPECT_TRUE(err == -2);

    EXPECT_TRUE(!bean1->getObjectId(testHelper.r1));
    err = bean1->setRelation(testHelper.r1, bean2->getId());
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == bean2->getId());

    err = bean1->setRelation(testHelper.r1, bean3->getId());
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r1) == bean3->getId());
}

TEST(Bean, array_relation)
{
    DummyBeanDB dummyDB;
    Value value;
    int err = 0;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    oidType beanId = 0;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    beanId = bean1->getObjectId(nullptr, -1);
    EXPECT_TRUE(beanId == 0);
    beanId = bean1->getObjectId(testHelper.p1, -1);
    EXPECT_TRUE(beanId == 0);
    beanId = bean1->getObjectId(testHelper.r_array_1, -1);
    EXPECT_TRUE(beanId == 0);
    beanId = bean1->getObjectId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId == 0);

    err = bean1->setRelationAt(nullptr, -1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelationAt(nullptr, -1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelationAt(testHelper.p1, -1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelationAt(testHelper.r_array_1, -1, bean2);
    EXPECT_TRUE(err == -4);

    err = bean1->append(nullptr, Json::Value::nullRef);
    EXPECT_TRUE(err == -1);
    err = bean1->append(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->append(testHelper.p1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(err == -4);

    err = bean1->addArray(nullptr);
    EXPECT_TRUE(err == -2);
    err = bean1->addArray(testHelper.p1);
    EXPECT_TRUE(err == -2);
    err = bean1->addArray(testHelper.r_array_1);
    EXPECT_TRUE(err == 0);

    err = bean1->setRelationAt(testHelper.r_array_1, -1, bean3);
    EXPECT_TRUE(err == -5);
    err = bean1->setRelationAt(testHelper.r_array_1, 0, bean3);
    EXPECT_TRUE(err == -5);

    err = bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 1);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean2->getId());
    err = bean1->appendRelation(testHelper.r_array_1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 2);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 1) == bean2->getId());

    err = bean1->setRelationAt(testHelper.r_array_1, 0, bean3);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 2);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean3->getId());
    err = bean1->setRelationAt(testHelper.r_array_1, 1, bean3->getId());
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 2);
    EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean3->getId());

    beanId = bean1->getObjectId(testHelper.r_array_1, 0);
    EXPECT_TRUE(beanId == bean3->getId());
    beanId = bean1->getObjectId(testHelper.r_array_1, 1);
    EXPECT_TRUE(beanId == bean3->getId());
    beanId = bean1->getObjectId(testHelper.r_array_1, 2);
    EXPECT_TRUE(beanId == 0);

    // err = bean1->appendRelation(testHelper.r_array_1, nullptr);
    // EXPECT_TRUE(err == -1);
    // err = bean1->append(nullptr, bean2);
    // EXPECT_TRUE(err == -2);
    // err = bean1->append(testHelper.p1, bean2);
    // EXPECT_TRUE(err == -2);
    // err = bean1->append(testHelper.r_array_1, bean2);
    // EXPECT_TRUE(err == -4);

    // err = bean1->setRelation(testHelper.r_array_1, 0, nullptr);
    // EXPECT_TRUE(err == -1);
    // err = bean1->setRelation(nullptr, 0, bean2);
    // EXPECT_TRUE(err == -2);
    // err = bean1->setRelation(testHelper.p1, 0, bean2);
    // EXPECT_TRUE(err == -2);
    // err = bean1->setRelation(testHelper.r_array_1, 0, bean2);
    // EXPECT_TRUE(err == -4);

    // EXPECT_TRUE(!bean1->getObjectId(testHelper.r_array_1, 0));
    // err = bean1->addArray(nullptr);
    // EXPECT_TRUE(err == -2);
    // err = bean1->addArray(testHelper.p1);
    // EXPECT_TRUE(err == -2);
    // err = bean1->addArray(testHelper.r_array_1);
    // EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 0);

    // err = bean1->append(testHelper.r_array_1, bean2);
    // EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(bean1->size(testHelper.r_array_1) == 1);

    // err = bean1->setRelation(testHelper.r_array_1, 1, bean2);
    // EXPECT_TRUE(err == -5);
    // err = bean1->setRelation(testHelper.r_array_1, 0, bean2);
    // EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean2->getId());
    // err = bean1->setRelation(testHelper.r_array_1, 0, bean3);
    // EXPECT_TRUE(err == 0);
    // EXPECT_TRUE(bean1->getObjectId(testHelper.r_array_1, 0) == bean3->getId());
    // bean1->set(testHelper.p1, 1);
    // EXPECT_TRUE(!bean1->getObjectId(testHelper.p1, 0));
    // EXPECT_TRUE(!bean1->getObjectId(testHelper.r_array_1, 1));

    //size()
    EXPECT_TRUE(bean1->size(nullptr) == 0);
    EXPECT_TRUE(bean1->size(testHelper.p1) == 0);

    //removeAt()
    value = bean3->removeAt(nullptr, 0);
    EXPECT_TRUE(value != 0);
    value = bean3->removeAt(testHelper.p1, 0);
    EXPECT_TRUE(value != 0);
    bean3->set(testHelper.p1, 1);
    value = bean3->removeAt(testHelper.p1, 0);
    EXPECT_TRUE(value != 0);
    value = bean3->removeAt(testHelper.p_array_int, 3);
    EXPECT_TRUE(value != 0);

    err = bean3->addArray(testHelper.r_array_1);
    err = bean3->appendRelation(testHelper.r_array_1, bean1);
    err = bean3->appendRelation(testHelper.r_array_1, bean2->getId());
    EXPECT_TRUE(bean3->size(testHelper.r_array_1) == 2);

    value = bean3->removeAt(testHelper.r_array_1, 0);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean3->size(testHelper.r_array_1) == 1);
    value = bean3->removeAt(testHelper.r_array_1, 0);
    EXPECT_TRUE(value == 0);
    EXPECT_TRUE(bean3->size(testHelper.r_array_1) == 0);
}

TEST(Bean, isMember)
{
    DummyBeanDB dummyDB;
    Value value;
    TestHelper testHelper;
    BeanWorld* world = nullptr;
    
    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world);
    
    Bean* bean1 = world->newBean();

    EXPECT_TRUE(!bean1->isMember(nullptr));

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(!bean1->isMember(testHelper.p1));
    bean1->set(testHelper.p1, 1);
    EXPECT_TRUE(bean1->isMember(testHelper.p1));
    EXPECT_TRUE(bean1->getMemberNames().size() ==1);

    EXPECT_TRUE(!bean1->isMember(testHelper.p2));
    bean1->set(testHelper.p2, 2);
    EXPECT_TRUE(bean1->isMember(testHelper.p2));
    EXPECT_TRUE(bean1->getMemberNames().size() == 2);

    EXPECT_TRUE(!bean1->isMember(testHelper.p_array_int));
    bean1->addArray(testHelper.p_array_int);
    EXPECT_TRUE(bean1->isMember(testHelper.p_array_int));
    EXPECT_TRUE(bean1->getMemberNames().size() == 3);

    Bean* bean2 = world->newBean();

    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->isMember(testHelper.r1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 4);

    EXPECT_TRUE(!bean1->isMember(testHelper.r_array_1));
    bean1->addArray(testHelper.r_array_1);
    EXPECT_TRUE(bean1->isMember(testHelper.r_array_1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 5);
}


TEST(Bean, save)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld* world = nullptr;

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;
    Json::Value value;

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1, false);
    setBeanNativeData(testHelper, bean1, false);

    bean2 = world->newBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2, false);
    setBeanNativeData(testHelper, bean2, false);

    bean3 = world->newBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3, false);
    setBeanNativeData(testHelper, bean3, false);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->addArray(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = bean1->save();
    EXPECT_TRUE(err == 0);
    err = bean2->save();
    EXPECT_TRUE(err == 0);
    err = bean3->save();
    EXPECT_TRUE(err == 0);

    testdb.disconnect();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, *world, false);

    bean1 = world->getBean(beanId_1);
    bean2 = world->getBean(beanId_2);
    bean3 = world->getBean(beanId_3);

    validateBean(testHelper, bean1);
    validateBean(testHelper, bean2);
    validateBean(testHelper, bean3);

    testdb.disconnect();
}

TEST(Bean, removeAllProperties)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld* world = nullptr;

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);

    bean2 = world->newBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);

    bean3 = world->newBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->addArray(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    bean1->save();
    bean2->save();
    bean3->save();

    EXPECT_TRUE(bean1->getMemberNames().size() > 0);
    EXPECT_TRUE(bean2->getMemberNames().size() > 0);
    EXPECT_TRUE(bean3->getMemberNames().size() > 0);

    bean1->removeAllProperties();
    bean2->removeAllProperties();
    bean3->removeAllProperties();

    EXPECT_TRUE(bean1->getMemberNames().size() == 0);
    EXPECT_TRUE(bean2->getMemberNames().size() == 0);
    EXPECT_TRUE(bean3->getMemberNames().size() == 0);

    testdb.disconnect();
}


TEST(Bean, nativeData)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld* world = nullptr;

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->newBean();
    setBeanNativeData(testHelper, bean1);
    validateBeanNativeData(testHelper, bean1);
    EXPECT_TRUE(bean1->getNativeData().getMemberNames().size() > 0);

    err = bean1->removeNativeData();
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(((const Bean*)bean1)->getNativeData().getMemberNames().size() == 0);

    testdb.disconnect();
}


TEST(Bean, clear)
{
    int err = 0;
    DummyBeanDB testdb;
    BeanWorld* world = nullptr;

   TestHelper testHelper;
    Bean* bean = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;
    oidType beanId_3 = 0;

    testdb.connect();
    world = testdb.getWorld();

    initTestHelper(testHelper, *world, false);

    bean1 = world->newBean();
    beanId_1 = bean1->getId();
    setBeanProperties(testHelper, bean1);
    setBeanNativeData(testHelper, bean1);

    bean2 = world->newBean();
    beanId_2 = bean2->getId();
    setBeanProperties(testHelper, bean2);
    setBeanNativeData(testHelper, bean2);

    bean3 = world->newBean();
    beanId_3 = bean3->getId();
    setBeanProperties(testHelper, bean3);
    setBeanNativeData(testHelper, bean3);

    bean3->setRelation(testHelper.r1, bean1);
    bean3->setRelation(testHelper.r2, bean2);
    bean3->addArray(testHelper.r_array_1);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);

    err = bean1->save();
    err = bean2->save();
    err = bean3->save();

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

    testdb.disconnect();
}


