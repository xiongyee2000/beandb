

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/bean/Property.h"
#include "org/jinsha/bean/Bean.h"
#include "org/jinsha/bean/BeanWorld.h"

#include "./common.h"
#include "./DummyBeanDB.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

TEST(Property, getName)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    world.defineProperty("p1", Property::IntType);
    Property* property = world.getProperty("p1");
    EXPECT_TRUE(property != nullptr);
    EXPECT_TRUE(property->getName() == "p1");
}

// TEST(Property, indexed)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world.defineProperty("p1", Property::IntType);
//     Property* property = world.getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
// }

// TEST(Property, create_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world.defineProperty("p1", Property::IntType);
//     Property* property = world.getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
// }

// TEST(Property, remove_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world.defineProperty("p1", Property::IntType);
//     Property* property = world.getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
//     property->removeIndex();
//     EXPECT_TRUE(property->indexed() == false);
// }


void test_getSubjects_common(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<oidType>& beans, bool needIndex = false)
{
    TestHelper testHelper;
    initTestHelper(testHelper, world, needIndex);

    testHelper.p_int->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_int64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_real->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_str->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_0->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.r1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.setProperty(testHelper.p_real, 1.0);
    bean1.setProperty(testHelper.p_str, "hello");
    bean1.setProperty(testHelper.p_bool_0, false);
    bean1.setProperty(testHelper.p_bool_1, true);
    bean1.setProperty(testHelper.p_int, 1);
    bean1.setProperty(testHelper.p_uint, 2U);
    bean1.setProperty(testHelper.p_int64, 3);
    bean1.setProperty(testHelper.p_uint64, 4U);
    bean1.setRelation(testHelper.r1, &bean2);

    testHelper.p_int->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_uint->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_int64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_uint64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_real->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_str->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_bool_0->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_bool_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.r1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);

    bean2.setProperty(testHelper.p_real, 1.0);
    bean2.setProperty(testHelper.p_str, "hello");
    bean2.setProperty(testHelper.p_bool_0, false);
    bean2.setProperty(testHelper.p_bool_1, true);
    bean2.setProperty(testHelper.p_int, 1);
    bean2.setProperty(testHelper.p_uint, 2U);
    bean2.setProperty(testHelper.p_int64, 3);
    bean2.setProperty(testHelper.p_uint64, 4U);
    bean2.setRelation(testHelper.r1, &bean1);

    testHelper.p_int->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_uint->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_int64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_uint64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_real->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_str->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_bool_0->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.p_bool_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    testHelper.r1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    EXPECT_TRUE(testHelper.r1->getNumOfSubjects() == 2);

    bean2.removeProperty(testHelper.p_real);
    bean2.removeProperty(testHelper.p_str);
    bean2.removeProperty(testHelper.p_bool_0);
    bean2.removeProperty(testHelper.p_bool_1);
    bean2.removeProperty(testHelper.p_int);
    bean2.removeProperty(testHelper.p_uint);
    bean2.removeProperty(testHelper.p_int64);
    bean2.removeProperty(testHelper.p_uint64);
    bean2.removeProperty(testHelper.r1);

    testHelper.p_int->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_uint->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_int64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_uint64->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_real->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_str->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_bool_0->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.p_bool_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    testHelper.r1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
}

void test_getSubjects_common_array(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<oidType>& beans)
{
    Property *pArray_1, *rArray_1;
   pArray_1 =  world.defineArrayProperty("p_array_int", Property::IntType);

    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineProperty("p_array_int");
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    pArray_1 = world.defineArrayProperty("p_array_int", Property::BoolType);

    bean1.createArrayProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    rArray_1 = world.defineArrayRelation("r_array_1");

    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.createArrayRelation(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineRelation("r_array_1");
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

}

// TEST(Property, getSubjects_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     std::list<oidType> beans;
//     TestHelper testHelper;

//     initTestHelper(testHelper, world);

//     Bean &bean1 = *world.createBean();
//     Bean& bean2 = *world.createBean();

//     test_getSubjects_common(world, bean1, bean2, beans, false);
// }

TEST(Property, getSubjects_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, world);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_getSubjects_common(world, bean1, bean2, beans, true);
}

// TEST(Property, getSubjects_array_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     std::list<oidType> beans;

//     Bean &bean1 = *world.createBean();
//     Bean& bean2 = *world.createBean();

//     test_getSubjects_common_array(world, bean1, bean2, beans, false);
// }

TEST(Property, getSubjects_array_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();

    test_getSubjects_common_array(world, bean1, bean2, beans);
}


void test_findSubjects_common(TestHelper& testHelper, BeanWorld &world, Bean& bean1, Bean& bean2, Bean& bean3, std::list<oidType>& beans, bool needIndex = false)
{
    testHelper.p_int->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_int64->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint64->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_real->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_str->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_0->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_1->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_array_int->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.r1->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.r_array_1->findSubjects(0, beans);
    EXPECT_TRUE(beans.size() == 0);

    auto bean3_id = bean3.getId();
    bean1.setRelation(testHelper.r1, &bean3);
    testHelper.r1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 1);
    bean2.setRelation(testHelper.r1, &bean3);
    testHelper.r1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 2);

    bean1.removeProperty(testHelper.r1);
    testHelper.r1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 1);

    bean2.removeProperty(testHelper.r1);
    testHelper.r1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.createArrayRelation(testHelper.r_array_1);
    bean2.createArrayRelation(testHelper.r_array_1);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 2);
    bean2.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 3);
    bean2.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 4);

    bean1.removeProperty(testHelper.r_array_1);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 2);
    bean2.removeProperty(testHelper.r_array_1);
    testHelper.r_array_1->findSubjects(bean3_id, beans);
    EXPECT_TRUE(beans.size() == 0);
}


// TEST(Property, findSubjects_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     std::list<oidType> beans;
//     TestHelper testHelper;

//     initTestHelper(testHelper, world, false);

//     Bean &bean1 = *world.createBean();
//     Bean& bean2 = *world.createBean();
//     Bean& bean3 = *world.createBean();

//     test_findSubjects_common(testHelper, world, bean1, bean2, bean3, beans, false);
// }

TEST(Property, findSubjects_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, world, true);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();
    Bean& bean3 = *world.createBean();

    test_findSubjects_common(testHelper, world, bean1, bean2, bean3, beans, true);
}


void test_findObjects_common(TestHelper& testHelper, BeanWorld &world, Bean& bean1, Bean& bean2, Bean& bean3, std::list<oidType>& beans, bool needIndex = false)
{
    // init_world(testHelper, world, needIndex);

    testHelper.p_int->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_int64->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint64->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_real->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_str->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_0->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_bool_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_array_int->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.r1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.setRelation(testHelper.r1, &bean3);
    testHelper.r1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean2.setRelation(testHelper.r1, &bean3);
    testHelper.r1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);

    bean1.removeProperty(testHelper.r1);
    testHelper.r1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);

    bean2.removeProperty(testHelper.r1);
    testHelper.r1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.createArrayRelation(testHelper.r_array_1);
    bean2.createArrayRelation(testHelper.r_array_1);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    bean1.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean2.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean2.appendRelation(testHelper.r_array_1, &bean3);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.appendRelation(testHelper.r_array_1, &bean2);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    bean2.appendRelation(testHelper.r_array_1, &bean1);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 3);
    EXPECT_TRUE(testHelper.r_array_1->getNumOfObjects() == 3);

    bean1.removeProperty(testHelper.r_array_1);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 2);
    bean2.removeProperty(testHelper.r_array_1);
    testHelper.r_array_1->getObjects(beans);
    EXPECT_TRUE(beans.size() == 0);
}


// TEST(Property, findObjects_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     std::list<oidType> beans;
//     TestHelper testHelper;

//     initTestHelper(testHelper, world);

//     Bean &bean1 = *world.createBean();
//     Bean& bean2 = *world.createBean();
//     Bean& bean3 = *world.createBean();

//     test_findObjects_common(testHelper, world, bean1, bean2, bean3, beans, false);
// }

TEST(Property, findObjects_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, world, true);

    Bean &bean1 = *world.createBean();
    Bean& bean2 = *world.createBean();
    Bean& bean3 = *world.createBean();

    test_findObjects_common(testHelper, world, bean1, bean2, bean3, beans, false);
}

void test_find_init(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_bool_0, false);
    bean1->setProperty(testHelper.p_bool_1, true);
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 2U);
    bean1->setProperty(testHelper.p_int64, 3);
    bean1->setProperty(testHelper.p_uint64, 4U);

    bean2->setProperty(testHelper.p_real, 1.0);
    bean2->setProperty(testHelper.p_str, "hello");
    bean2->setProperty(testHelper.p_bool_0, false);
    bean2->setProperty(testHelper.p_bool_1, true);
    bean2->setProperty(testHelper.p_int, 1);
    bean2->setProperty(testHelper.p_uint, 2U);
    bean2->setProperty(testHelper.p_int64, 3);
    bean2->setProperty(testHelper.p_uint64, 4U);
}

void test_findEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2)
{
    std::list<oidType> beans;

    test_find_init(testHelper, world, bean1, bean2, nullptr);

    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) == 2U);
    }
}

// TEST(Property, findEqual_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     TestHelper testHelper;
//     initTestHelper(testHelper, world);
//     Bean *bean1 = world.createBean();
//     Bean *bean2 = world.createBean();
//     test_findEqual_common(testHelper, world, bean1, bean2);
// }

TEST(Property, findEqual_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    TestHelper testHelper;
    initTestHelper(testHelper, world);
    Bean *bean1 = world.createBean();
    Bean *bean2 = world.createBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
     test_findEqual_common(testHelper, world, bean1, bean2);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
     test_findEqual_common(testHelper, world, bean1, bean2);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
     test_findEqual_common(testHelper, world, bean1, bean2);
}

void test_findLessEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;
    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 1U);
    bean1->setProperty(testHelper.p_int64, 101);
    bean1->setProperty(testHelper.p_uint64, 101U);

    bean2->setProperty(testHelper.p_real, 2.0);
    bean2->setProperty(testHelper.p_str, "my");
    bean2->setProperty(testHelper.p_int, 2);
    bean2->setProperty(testHelper.p_uint, 2U);
    bean2->setProperty(testHelper.p_int64, 102);
    bean2->setProperty(testHelper.p_uint64, 102U);

    bean3->setProperty(testHelper.p_real, 3.0);
    bean3->setProperty(testHelper.p_str, "world");
    bean3->setProperty(testHelper.p_int, 3);
    bean3->setProperty(testHelper.p_uint, 3U);
    bean3->setProperty(testHelper.p_int64, 103);
    bean3->setProperty(testHelper.p_uint64, 103U);


    testHelper.p_real->findLessEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) <= 1.0);
    }
    testHelper.p_real->findLessEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) <= 2.0);
    }
    testHelper.p_real->findLessEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) <= 3.0);
    }

    testHelper.p_str->findLessEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("hello") <= 0);
    }
    testHelper.p_str->findLessEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("my") <= 0);
    }
    testHelper.p_str->findLessEqual("world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare( "world") <= 0);
    }

   testHelper.p_int->findLessEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) <= 1);
    }
   testHelper.p_int->findLessEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) <= 2);
    }
   testHelper.p_int->findLessEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) <= 3);
    }

    testHelper.p_uint->findLessEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) <= (uint_t)1);
    }
    testHelper.p_uint->findLessEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) <= (uint_t)2);
    }
    testHelper.p_uint->findLessEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) <= (uint_t)3);
    }
}

// TEST(Property, findLessEqual_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     TestHelper testHelper;
//     initTestHelper(testHelper, world);

//     Bean* bean1 = world.createBean();
//     Bean* bean2 = world.createBean();
//     Bean* bean3 = world.createBean();

//     test_findLessEqual_common(testHelper, world, bean1, bean2, bean3);
// }

TEST(Property, findLessEqual_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    TestHelper testHelper;
    initTestHelper(testHelper, world);

    Bean* bean1 = world.createBean();
    // bean1->setProperty(testHelper.p_real, 1.0);
    // bean1->setProperty(testHelper.p_str, "hello");
    // bean1->setProperty(testHelper.p_int, 1);
    // bean1->setProperty(testHelper.p_uint, 1U);
    // bean1->setProperty(testHelper.p_int64, 101);
    // bean1->setProperty(testHelper.p_uint64, 101U);

    Bean* bean2 = world.createBean();
    // bean2->setProperty(testHelper.p_real, 2.0);
    // bean2->setProperty(testHelper.p_str, "my");
    // bean2->setProperty(testHelper.p_int, 2);
    // bean2->setProperty(testHelper.p_uint, 2U);
    // bean2->setProperty(testHelper.p_int64, 102);
    // bean2->setProperty(testHelper.p_uint64, 102U);

    Bean* bean3 = world.createBean();
    // bean3->setProperty(testHelper.p_real, 3.0);
    // bean3->setProperty(testHelper.p_str, "world");
    // bean3->setProperty(testHelper.p_int, 3);
    // bean3->setProperty(testHelper.p_uint, 3U);
    // bean3->setProperty(testHelper.p_int64, 103);
    // bean3->setProperty(testHelper.p_uint64, 103U);
    
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessEqual_common(testHelper, world, bean1, bean2, bean3);
    
    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findLessEqual_common(testHelper, world, bean1, bean2, bean3);
    
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessEqual_common(testHelper, world, bean1, bean2, bean3);
}

void test_FindGreaterEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;
    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 1U);
    bean1->setProperty(testHelper.p_int64, 101);
    bean1->setProperty(testHelper.p_uint64, 101U);

    bean2->setProperty(testHelper.p_real, 2.0);
    bean2->setProperty(testHelper.p_str, "my");
    bean2->setProperty(testHelper.p_int, 2);
    bean2->setProperty(testHelper.p_uint, 2U);
    bean2->setProperty(testHelper.p_int64, 102);
    bean2->setProperty(testHelper.p_uint64, 102U);

    bean3->setProperty(testHelper.p_real, 3.0);
    bean3->setProperty(testHelper.p_str, "world");
    bean3->setProperty(testHelper.p_int, 3);
    bean3->setProperty(testHelper.p_uint, 3U);
    bean3->setProperty(testHelper.p_int64, 103);
    bean3->setProperty(testHelper.p_uint64, 103U);

    testHelper.p_real->findGreaterEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) >= 1.0);
    }
    testHelper.p_real->findGreaterEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) >= 2.0);
    }
    testHelper.p_real->findGreaterEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) >= 3.0);
    }

    testHelper.p_str->findGreaterEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("hello") >= 0);
    }
    testHelper.p_str->findGreaterEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("my") >= 0);
    }
    testHelper.p_str->findGreaterEqual("world", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare( "world") >= 0);
    }

   testHelper.p_int->findGreaterEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) >= 1);
    }
   testHelper.p_int->findGreaterEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) >= 2);
    }
   testHelper.p_int->findGreaterEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) >= 3);
    }

    testHelper.p_uint->findGreaterEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) >= (uint_t)1);
    }
    testHelper.p_uint->findGreaterEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) >= (uint_t)2);
    }
    testHelper.p_uint->findGreaterEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) >= (uint_t)3);
    }
}

// TEST(Property, findGreaterEqual_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     TestHelper testHelper;
//     initTestHelper(testHelper, world);

//     Bean* bean1 = world.createBean();
//     Bean* bean2 = world.createBean();
//     Bean* bean3 = world.createBean();

//     test_FindGreaterEqual_common(testHelper, world, bean1, bean2, bean3);
// }

TEST(Property, findGreaterEqual_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_FindGreaterEqual_common(testHelper, world, bean1, bean2, bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_FindGreaterEqual_common(testHelper, world, bean1, bean2, bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_FindGreaterEqual_common(testHelper, world, bean1, bean2, bean3);
}

void test_findLessThan_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;

    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 1U);
    bean1->setProperty(testHelper.p_int64, 101);
    bean1->setProperty(testHelper.p_uint64, 101U);

    bean2->setProperty(testHelper.p_real, 2.0);
    bean2->setProperty(testHelper.p_str, "my");
    bean2->setProperty(testHelper.p_int, 2);
    bean2->setProperty(testHelper.p_uint, 2U);
    bean2->setProperty(testHelper.p_int64, 102);
    bean2->setProperty(testHelper.p_uint64, 102U);

    bean3->setProperty(testHelper.p_real, 3.0);
    bean3->setProperty(testHelper.p_str, "world");
    bean3->setProperty(testHelper.p_int, 3);
    bean3->setProperty(testHelper.p_uint, 3U);
    bean3->setProperty(testHelper.p_int64, 103);
    bean3->setProperty(testHelper.p_uint64, 103U);


    testHelper.p_real->findLessThan(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    testHelper.p_real->findLessThan(2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) < 2.0);
    }
    testHelper.p_real->findLessThan(3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) < 3.0);
    }
    testHelper.p_real->findLessThan(4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) < 4.0);
    }

    testHelper.p_str->findLessThan("hello", beans);
   EXPECT_TRUE(beans.size() == 0);
    testHelper.p_str->findLessThan("my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("my") < 0);
    }
    testHelper.p_str->findLessThan("world", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare( "world") < 0);
    }
    testHelper.p_str->findLessThan("zzz", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("zzz") < 0);
    }

   testHelper.p_int->findLessThan((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 0);
   testHelper.p_int->findLessThan((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) < 2);
    }
   testHelper.p_int->findLessThan((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) < 3);
    }
   testHelper.p_int->findLessThan(4, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) < 4);
    }

    testHelper.p_uint->findLessThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint->findLessThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) < (uint_t)2);
    }
    testHelper.p_uint->findLessThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) < (uint_t)3);
    }
    testHelper.p_uint->findLessThan((uint_t)4, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) < (uint_t)4);
    }
}

// TEST(Property, findLessThan_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     TestHelper testHelper;
//     initTestHelper(testHelper, world);

//     Bean* bean1 = world.createBean();
//     Bean* bean2 = world.createBean();
//     Bean* bean3 = world.createBean();
    
//     test_findLessThan_common(testHelper, world, bean1, bean2, bean3);
// }

TEST(Property, findLessThan_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    TestHelper testHelper;
    initTestHelper(testHelper, world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessThan_common(testHelper, world, bean1, bean2, bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findLessThan_common(testHelper, world, bean1, bean2, bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessThan_common(testHelper, world, bean1, bean2, bean3);
}

void test_findGreaterThan_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;

    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 1U);
    bean1->setProperty(testHelper.p_int64, 101);
    bean1->setProperty(testHelper.p_uint64, 101U);

    bean2->setProperty(testHelper.p_real, 2.0);
    bean2->setProperty(testHelper.p_str, "my");
    bean2->setProperty(testHelper.p_int, 2);
    bean2->setProperty(testHelper.p_uint, 2U);
    bean2->setProperty(testHelper.p_int64, 102);
    bean2->setProperty(testHelper.p_uint64, 102U);

    bean3->setProperty(testHelper.p_real, 3.0);
    bean3->setProperty(testHelper.p_str, "world");
    bean3->setProperty(testHelper.p_int, 3);
    bean3->setProperty(testHelper.p_uint, 3U);
    bean3->setProperty(testHelper.p_int64, 103);
    bean3->setProperty(testHelper.p_uint64, 103U);


    testHelper.p_real->findGreaterThan(0.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) > 0.0);
    }
    testHelper.p_real->findGreaterThan(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) > 1.0);
    }
    testHelper.p_real->findGreaterThan(2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) > 2.0);
    }
    testHelper.p_real->findGreaterThan(3.0, beans);
   EXPECT_TRUE(beans.size() == 0);

    testHelper.p_str->findGreaterThan("a", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("a") > 0);
    }
    testHelper.p_str->findGreaterThan("hello", beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("hello") > 0);
    }
    testHelper.p_str->findGreaterThan("my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare("my") > 0);
    }
    testHelper.p_str->findGreaterThan("world", beans);
   EXPECT_TRUE(beans.size() == 0);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str).compare( "world") > 0);
    }

    testHelper.p_int->findGreaterThan(0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) > 0);
    }

   testHelper.p_int->findGreaterThan(1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) > 1);
    }
   testHelper.p_int->findGreaterThan(2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) > 2);
    }
   testHelper.p_int->findGreaterThan(3, beans);
   EXPECT_TRUE(beans.size() == 0);

    testHelper.p_uint->findGreaterThan((uint_t)0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) > (uint_t)0);
    }
    testHelper.p_uint->findGreaterThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) > (uint_t)1);
    }
    testHelper.p_uint->findGreaterThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) > (uint_t)2);
    }
    testHelper.p_uint->findGreaterThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 0);
}

// TEST(Property, findGreaterThan_without_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);
//     std::list<oidType> beans;
//     TestHelper testHelper;

//     initTestHelper(testHelper, world);

//     Bean* bean1 = world.createBean();
//     Bean* bean2 = world.createBean();
//     Bean* bean3 = world.createBean();
//     test_findGreaterThan_common(testHelper, world, bean1, bean2 ,bean3);
// }

TEST(Property, findGreaterThan_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    TestHelper testHelper;
    initTestHelper(testHelper, world);

    Bean* bean1 = world.createBean();
    Bean* bean2 = world.createBean();
    Bean* bean3 = world.createBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findGreaterThan_common(testHelper, world, bean1, bean2 ,bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findGreaterThan_common(testHelper, world, bean1, bean2 ,bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findGreaterThan_common(testHelper, world, bean1, bean2 ,bean3);
}

TEST(Property, index_more_complicated)
{
    DummyBeanDB dummyDB;
    BeanWorld world((AbstractBeanDB&)dummyDB);
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, world);

    ///////////////////////////////////////////////////////
    //createIndex()
    ///////////////////////////////////////////////////////
    Bean* bean1 = world.createBean();
    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_bool_0, false);
    bean1->setProperty(testHelper.p_bool_1, true);
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 2U);

    Bean* bean2 = world.createBean();
    bean2->setProperty(testHelper.p_real, 1.0);
    bean2->setProperty(testHelper.p_str, "hello");
    bean2->setProperty(testHelper.p_bool_0, false);
    bean2->setProperty(testHelper.p_bool_1, true);
    bean2->setProperty(testHelper.p_int, 1);
    bean2->setProperty(testHelper.p_uint, 2U);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_bool_0->createIndex();
    testHelper.p_bool_1->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();

    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //removeIndex()
    ///////////////////////////////////////////////////////
    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_bool_0->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();

    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //reset property after createIndex()
    ///////////////////////////////////////////////////////
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_bool_0->createIndex();
    testHelper.p_bool_1->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    
    bean1->setProperty(testHelper.p_real, 1.0);
    bean1->setProperty(testHelper.p_str, "hello");
    bean1->setProperty(testHelper.p_bool_0, false);
    bean1->setProperty(testHelper.p_bool_1, true);
    bean1->setProperty(testHelper.p_int, 1);
    bean1->setProperty(testHelper.p_uint, 2U);

   testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //remove property after createIndex()
    ///////////////////////////////////////////////////////
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_bool_0->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();

    bean1->removeProperty(testHelper.p_real);
    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->getProperty(testHelper.p_real) == 1.0);
    }

    ///////////////////////////////////////////////////////
    //remove bean after createIndex()
    ///////////////////////////////////////////////////////
    world.removeBean(bean2->getId());
    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);

}