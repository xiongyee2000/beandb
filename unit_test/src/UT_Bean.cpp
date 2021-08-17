

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

    Bean *bean = new Bean(&world);
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

    EXPECT_TRUE(!bean1->isMember(testHelper.pArray_1));
    bean1->createArrayProperty(testHelper.pArray_1);
    EXPECT_TRUE(bean1->isMember(testHelper.pArray_1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 3);

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->isMember(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->isMember(testHelper.r1));
    EXPECT_TRUE(bean1->getMemberNames().size() == 4);

    EXPECT_TRUE(!bean1->isMember(testHelper.rArray_1));
    bean1->createArrayRelation(testHelper.rArray_1);
    EXPECT_TRUE(bean1->isMember(testHelper.rArray_1));
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

    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.pArray_1));
    err = bean1->createArrayProperty(testHelper.pArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.pArray_1));
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.pArray_1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.pArray_1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.pArray_1));

    Bean* bean2 = world.createBean();

    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, nullptr);
    EXPECT_TRUE(!bean1->hasRelation(testHelper.r1));
    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.r1));
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1));
    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.r1));

    EXPECT_TRUE(!bean1->hasArrayRelation(testHelper.rArray_1));
    bean1->createArrayRelation(testHelper.rArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(!bean1->hasProperty(testHelper.rArray_1));
    EXPECT_TRUE(!bean1->hasArrayProperty(testHelper.rArray_1));
    EXPECT_TRUE(!bean1->hasRelation(testHelper.rArray_1));
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.rArray_1));

    value = bean1->removeProperty(nullptr);
    EXPECT_TRUE(value.isNull());

    bean1->setProperty(testHelper.p1, 1);
    value = bean1->removeProperty(testHelper.p1);
    EXPECT_TRUE(value == 1);
    EXPECT_TRUE(bean1->hasProperty(testHelper.p1) == false);

    bean1->createArrayProperty(testHelper.pArray_1);
    bean1->appendProperty(testHelper.pArray_1, 1);
    value = bean1->removeProperty(testHelper.pArray_1);
    EXPECT_TRUE(value[0] == 1);
    EXPECT_TRUE(bean1->hasArrayProperty(testHelper.p1) == false);

    auto memberSize = bean1->getMemberNames().size();
    bean1->removeProperty(nullptr);
    EXPECT_TRUE(memberSize == bean1->getMemberNames().size());

    bean1->setRelation(testHelper.r1, bean2);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == true);
    bean1->removeProperty(testHelper.r1);
    EXPECT_TRUE(bean1->hasRelation(testHelper.r1) == false);

    bean1->createArrayRelation(testHelper.rArray_1);
    bean1->appendRelation(testHelper.rArray_1, bean2);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.rArray_1) == true);
    bean1->removeProperty(testHelper.rArray_1);
    EXPECT_TRUE(bean1->hasArrayRelation(testHelper.rArray_1) == false);
}

TEST(Bean, property)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    Value value;
    int err = 0;
    TestHelper testHelper;

    init_world(testHelper, world);

    world.defineProperty("p1", Property::IntType);

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
    world.defineProperty("p1", Property::UIntType);
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
    world.defineProperty("p1", Property::RealType);
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
    world.defineProperty("p1", Property::StringType);

    bean.setProperty(testHelper.p1, "v1");
    EXPECT_TRUE(bean.getProperty(testHelper.p1) == "v1");
    value = bean.removeProperty(testHelper.p1);
    EXPECT_TRUE(bean.hasProperty(testHelper.p1) == false);
    EXPECT_TRUE(value == "v1");

    bean.setProperty(testHelper.p1, "v1");
    world.undefineProperty("p2");
    world.defineProperty("p2", Property::StringType);
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
    world.defineProperty("p1", Property::BoolType);
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
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.pArray_1) == false);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 0);
    Value value2 = bean.getArrayProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value2.isNull());

    err = bean.appendProperty(testHelper.pArray_1, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.appendProperty(nullptr, 0);
    EXPECT_TRUE(err == -2);

    err = bean.setProperty(testHelper.pArray_1, 0, Value::null);
    EXPECT_TRUE(err == -1);
    err = bean.setProperty(nullptr, 0, 1);
    EXPECT_TRUE(err == -2);

    property = world.defineArrayProperty("pArray_1", Property::IntType);
    EXPECT_TRUE(property == world.getProperty("pArray_1"));
    err = bean.appendProperty(testHelper.pArray_1, 1);
    EXPECT_TRUE(err == -4);
    err = bean.setProperty(testHelper.pArray_1, 0, 1);
    EXPECT_TRUE(err == -4);

    //create empty array
    err = bean.createArrayProperty(testHelper.pArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.hasArrayProperty(testHelper.pArray_1) == true);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 0);
    value = bean.getArrayProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value.isNull());

    //append
    err = bean.appendProperty(testHelper.pArray_1, "0");
    EXPECT_TRUE(err == -3);
    err = bean.appendProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 1);
    value = bean.getArrayProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value == 0);
    value = bean.getArrayProperty(testHelper.pArray_1, 1);
    EXPECT_TRUE(value.isNull());
    err = bean.appendProperty(testHelper.pArray_1, 1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 2);
    value = bean.getArrayProperty(testHelper.pArray_1, 1);
    EXPECT_TRUE(value == 1);

    //setProperty()
    err = bean.setProperty(testHelper.pArray_1, 99, 99);
    EXPECT_TRUE(err == -5);
    err = bean.setProperty(testHelper.pArray_1, 0, 99);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 2);
    value = bean.getArrayProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value == 99);
    err = bean.setProperty(testHelper.pArray_1, 1, 999);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 2);
    value = bean.getArrayProperty(testHelper.pArray_1, 1);
    EXPECT_TRUE(value == 999);

    //removeProperty()
    value = bean.removeProperty(nullptr, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    bean.setProperty(testHelper.p1, 1);
    value = bean.removeProperty(testHelper.p1, 0);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.pArray_1, 3);
    EXPECT_TRUE(value.isNull());
    value = bean.removeProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value == 99);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 1);
    value = bean.removeProperty(testHelper.pArray_1, 0);
    EXPECT_TRUE(value == 999);
    EXPECT_TRUE(bean.getArraySize(testHelper.pArray_1) == 0);
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

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.pArray_1));
    bean1->createArrayProperty(testHelper.pArray_1);
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.pArray_1));

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

    err = bean1->appendRelation(testHelper.rArray_1, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->appendRelation(nullptr, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(testHelper.p1, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->appendRelation(testHelper.rArray_1, bean2);
    EXPECT_TRUE(err == -4);

    err = bean1->setRelation(testHelper.rArray_1, 0, nullptr);
    EXPECT_TRUE(err == -1);
    err = bean1->setRelation(nullptr, 0, bean2);
    EXPECT_TRUE(err == -2);
    err = bean1->setRelation(testHelper.p1, 0, bean2);
    EXPECT_TRUE(err == -3);
    err = bean1->setRelation(testHelper.rArray_1, 0, bean2);
    EXPECT_TRUE(err == -4);

    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.rArray_1, 0));
    err = bean1->createArrayRelation(nullptr);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(testHelper.p1);
    EXPECT_TRUE(err == -2);
    err = bean1->createArrayRelation(testHelper.rArray_1);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(testHelper.rArray_1) == 0);

    err = bean1->appendRelation(testHelper.rArray_1, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getArraySize(testHelper.rArray_1) == 1);

    err = bean1->setRelation(testHelper.rArray_1, 1, bean2);
    EXPECT_TRUE(err == -5);
    err = bean1->setRelation(testHelper.rArray_1, 0, bean2);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(testHelper.rArray_1, 0) == bean2->getId());
    err = bean1->setRelation(testHelper.rArray_1, 0, bean3);
    EXPECT_TRUE(err == 0);
    EXPECT_TRUE(bean1->getRelationBeanId(testHelper.rArray_1, 0) == bean3->getId());
    bean1->setProperty(testHelper.p1, 1);
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.p1, 0));
    EXPECT_TRUE(!bean1->getRelationBeanId(testHelper.rArray_1, 1));

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
    value = bean3->removeProperty(testHelper.pArray_1, 3);
    EXPECT_TRUE(value.isNull());

    err = bean3->createArrayRelation(testHelper.rArray_1);
    err = bean3->appendRelation(testHelper.rArray_1, bean1);
    err = bean3->appendRelation(testHelper.rArray_1, bean2);
    EXPECT_TRUE(bean3->getArraySize(testHelper.rArray_1) == 2);

    value = bean3->removeProperty(testHelper.rArray_1, 0);
    EXPECT_TRUE(value == bean1->getId());
    EXPECT_TRUE(bean3->getArraySize(testHelper.rArray_1) == 1);
    value = bean3->removeProperty(testHelper.rArray_1, 0);
    EXPECT_TRUE(value == bean2->getId());
    EXPECT_TRUE(bean3->getArraySize(testHelper.rArray_1) == 0);
}


