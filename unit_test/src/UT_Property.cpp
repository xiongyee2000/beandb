

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public
#include "org/jinsha/beandb/Property.h"
#include "org/jinsha/beandb/Bean.h"
#include "org/jinsha/beandb/BeanWorld.h"

#include "./common.h"
#include "./DummyBeanDB.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::beandb;

TEST(Property, getName)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    world->defineProperty("p1", Property::IntType);
    Property* property = world->getProperty("p1");
    EXPECT_TRUE(property != nullptr);
    EXPECT_TRUE(property->getName() == "p1");
}

// TEST(Property, indexed)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world->defineProperty("p1", Property::IntType);
//     Property* property = world->getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
// }

// TEST(Property, create_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world->defineProperty("p1", Property::IntType);
//     Property* property = world->getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
// }

// TEST(Property, remove_index)
// {
//     DummyBeanDB dummyDB;
//     BeanWorld world((AbstractBeanDB&)dummyDB);

//     world->defineProperty("p1", Property::IntType);
//     Property* property = world->getProperty("p1");
//     EXPECT_TRUE(property->indexed() == false);
//     property->createIndex();
//     EXPECT_TRUE(property->indexed() == true);
//     property->removeIndex();
//     EXPECT_TRUE(property->indexed() == false);
// }



TEST(Property, getSubjects_without_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world, false);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();

    test_getSubjects_common(*world, *bean1, *bean2, beans, false);
}

TEST(Property, getSubjects_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    // initTestHelper(testHelper, *world, true);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();

    test_getSubjects_common(*world, *bean1, *bean2, beans, true);
}

TEST(Property, getSubjects_array_without_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();

    test_getSubjects_common_array(*world, bean1, bean2, beans, false);
}

TEST(Property, getSubjects_array_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();

    test_getSubjects_common_array(*world, bean1, bean2, beans, true);
}



TEST(Property, findSubjects_without_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    // initTestHelper(testHelper, *world, false);

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();
    Bean& bean3 = *world->newBean();

    test_findSubjects_common(testHelper, *world, bean1, bean2, bean3, beans, false);
}

TEST(Property, findSubjects_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    // initTestHelper(testHelper, *world, true);

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();
    Bean& bean3 = *world->newBean();

    test_findSubjects_common(testHelper, *world, bean1, bean2, bean3, beans, true);
}



TEST(Property, findObjects_without_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    // initTestHelper(testHelper, *world, false);

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();
    Bean& bean3 = *world->newBean();

    test_findObjects_common(testHelper, *world, bean1, bean2, bean3, beans, false);
}

TEST(Property, findObjects_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    // initTestHelper(testHelper, *world, true);

    Bean &bean1 = *world->newBean();
    Bean& bean2 = *world->newBean();
    Bean& bean3 = *world->newBean();

    test_findObjects_common(testHelper, *world, bean1, bean2, bean3, beans, true);
}



TEST(Property, findEqual_without_index)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    TestHelper testHelper;
    initTestHelper(testHelper, *world, false);
    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();
    test_findEqual_common(testHelper, *world, bean1, bean2);
}

TEST(Property, findEqual_with_index)
{
    DummyBeanDB dummyDB;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world, false);

    Bean *bean1 = world->newBean();
    Bean *bean2 = world->newBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
     test_findEqual_common(testHelper, *world, bean1, bean2);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
     test_findEqual_common(testHelper, *world, bean1, bean2);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
     test_findEqual_common(testHelper, *world, bean1, bean2);
}

TEST(Property, findLessEqual_without_index)
{
    DummyBeanDB dummyDB;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world, false);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    test_findLessEqual_common(testHelper, *world, bean1, bean2, bean3);
}

TEST(Property, findLessEqual_with_index)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    TestHelper testHelper;
    initTestHelper(testHelper, *world, true);

    Bean* bean1 = world->newBean();
    // bean1->set(testHelper.p_real, 1.0);
    // bean1->set(testHelper.p_str, "hello");
    // bean1->set(testHelper.p_int, 1);
    // bean1->set(testHelper.p_uint, 1U);
    // bean1->set(testHelper.p_int64, 101);
    // bean1->set(testHelper.p_uint64, 101U);

    Bean* bean2 = world->newBean();
    // bean2->set(testHelper.p_real, 2.0);
    // bean2->set(testHelper.p_str, "my");
    // bean2->set(testHelper.p_int, 2);
    // bean2->set(testHelper.p_uint, 2U);
    // bean2->set(testHelper.p_int64, 102);
    // bean2->set(testHelper.p_uint64, 102U);

    Bean* bean3 = world->newBean();
    // bean3->set(testHelper.p_real, 3.0);
    // bean3->set(testHelper.p_str, "world");
    // bean3->set(testHelper.p_int, 3);
    // bean3->set(testHelper.p_uint, 3U);
    // bean3->set(testHelper.p_int64, 103);
    // bean3->set(testHelper.p_uint64, 103U);
    
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessEqual_common(testHelper, *world, bean1, bean2, bean3);
    
    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findLessEqual_common(testHelper, *world, bean1, bean2, bean3);
    
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessEqual_common(testHelper, *world, bean1, bean2, bean3);
}


TEST(Property, findGreaterEqual_without_index)
{
    DummyBeanDB dummyDB;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world, false);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    test_FindGreaterEqual_common(testHelper, *world, bean1, bean2, bean3);
}

TEST(Property, findGreaterEqual_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world, true);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_FindGreaterEqual_common(testHelper, *world, bean1, bean2, bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_FindGreaterEqual_common(testHelper, *world, bean1, bean2, bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_FindGreaterEqual_common(testHelper, *world, bean1, bean2, bean3);
}


TEST(Property, findLessThan_without_index)
{
    DummyBeanDB dummyDB;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();
    initTestHelper(testHelper, *world, false);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    
    test_findLessThan_common(testHelper, *world, bean1, bean2, bean3);
}

TEST(Property, findLessThan_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world, true);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessThan_common(testHelper, *world, bean1, bean2, bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findLessThan_common(testHelper, *world, bean1, bean2, bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findLessThan_common(testHelper, *world, bean1, bean2, bean3);
}

TEST(Property, findGreaterThan_without_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world, false);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    test_findGreaterThan_common(testHelper, *world, bean1, bean2 ,bean3);
}

TEST(Property, findGreaterThan_with_index)
{
    DummyBeanDB dummyDB;
    std::list<oidType> beans;
    TestHelper testHelper;
    BeanWorld* world = nullptr;

    dummyDB.connect();
    world = dummyDB.getWorld();

    initTestHelper(testHelper, *world, true);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findGreaterThan_common(testHelper, *world, bean1, bean2 ,bean3);

    testHelper.p_real->removeIndex();
    testHelper.p_str->removeIndex();
    testHelper.p_int->removeIndex();
    testHelper.p_uint->removeIndex();
    testHelper.p_int64->removeIndex();
    testHelper.p_uint64->removeIndex();
    test_findGreaterThan_common(testHelper, *world, bean1, bean2 ,bean3);

    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();
    testHelper.p_int64->createIndex();
    testHelper.p_uint64->createIndex();
    test_findGreaterThan_common(testHelper, *world, bean1, bean2 ,bean3);
}

TEST(Property, index_more_complicated)
{
    DummyBeanDB dummyDB;
    BeanWorld *world = nullptr;
    dummyDB.connect();
    world = dummyDB.getWorld();
    std::list<oidType> beans;
    TestHelper testHelper;

    initTestHelper(testHelper, *world);

    ///////////////////////////////////////////////////////
    //createIndex()
    ///////////////////////////////////////////////////////
    Bean* bean1 = world->newBean();
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_bool_0, false);
    bean1->set(testHelper.p_bool_1, true);
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 2U);

    Bean* bean2 = world->newBean();
    bean2->set(testHelper.p_real, 1.0);
    bean2->set(testHelper.p_str, "hello");
    bean2->set(testHelper.p_bool_0, false);
    bean2->set(testHelper.p_bool_1, true);
    bean2->set(testHelper.p_int, 1);
    bean2->set(testHelper.p_uint, 2U);

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
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_uint) == 2U);
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
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_uint) == 2U);
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
    
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_bool_0, false);
    bean1->set(testHelper.p_bool_1, true);
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 2U);

   testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_uint) == 2U);
    }

    ///////////////////////////////////////////////////////
    //remove property after createIndex()
    ///////////////////////////////////////////////////////
    testHelper.p_real->createIndex();
    testHelper.p_str->createIndex();
    testHelper.p_bool_0->createIndex();
    testHelper.p_int->createIndex();
    testHelper.p_uint->createIndex();

    bean1->remove(testHelper.p_real);
    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world->getBean(beanId)->get(testHelper.p_real) == 1.0);
    }

    ///////////////////////////////////////////////////////
    //remove bean after createIndex()
    ///////////////////////////////////////////////////////
    world->unloadBean(bean2);
    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);

}