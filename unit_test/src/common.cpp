#include <gmock/gmock.h>


//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public

#include "org/jinsha/bean/Property.h"
#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////

void initTestHelper(TestHelper& testHelper, BeanWorld& world, bool needIndex)
{
    testHelper.p_int = world.defineProperty("p_int", Property::IntType);
    testHelper.p_uint = world.defineProperty("p_uint", Property::UIntType);
    testHelper.p_int64 = world.defineProperty("p_int64", Property::IntType);
    testHelper.p_uint64 = world.defineProperty("p_uint64", Property::UIntType);
    testHelper.p_real = world.defineProperty("p_real", Property::RealType);
    testHelper.p_str = world.defineProperty("p_str", Property::StringType);
    testHelper.p_bool_0 = world.defineProperty("p_bool_0", Property::BoolType);
    testHelper.p_bool_1 = world.defineProperty("p_bool_1", Property::BoolType);

    testHelper.p1 = world.defineProperty("p1", Property::IntType);
    testHelper.p2 = world.defineProperty("p2", Property::IntType);
    testHelper.p_array_int = world.defineArrayProperty("p_array_int", Property::IntType);
    testHelper.p_array_uint = world.defineArrayProperty("p_array_uint", Property::UIntType);
    testHelper.p_array_real = world.defineArrayProperty("p_array_real", Property::RealType);
    testHelper.p_array_bool = world.defineArrayProperty("p_array_bool", Property::BoolType);
    testHelper.p_array_str = world.defineArrayProperty("p_array_str", Property::StringType);
    testHelper.r1 = world.defineRelation("r1");
    testHelper.r2 = world.defineRelation("r2");
    testHelper.r_array_1 =  world.defineArrayRelation("r_array_1");
    testHelper.r_array_2 =  world.defineArrayRelation("r_array_2");

    if (needIndex)
    {
        testHelper.p_real->createIndex();
        testHelper.p_str->createIndex();
        testHelper.p_int->createIndex();
        testHelper.p_uint->createIndex();
        testHelper.p_int64->createIndex();
        testHelper.p_uint64->createIndex();
        testHelper.p_bool_0->createIndex();
        testHelper.p_bool_1->createIndex();
        testHelper.p1->createIndex();
        testHelper.p2->createIndex();
        testHelper.p_array_int->createIndex();
        testHelper.r1->createIndex();
        testHelper.r2->createIndex();
        testHelper.r_array_1->createIndex();
        testHelper.r_array_2->createIndex();
    }
}


// void evaluate_testdb_empty_property(AbstractBeanDB& testdb)
// {
//     testdb.connect();
//     std::unordered_map<std::string, Property*> propertyMap;
//     testdb.loadProperties_(propertyMap);
//     EXPECT_TRUE(propertyMap.size() == 0);
//     testdb.disconnect();
// }


void setBeanProperties(TestHelper& testHelper, Bean* bean, bool saveAtOnce)
{
    // bean->set(testHelper.p_int, Json::Value::minInt);
    bean->set(testHelper.p_int, -1, saveAtOnce);
    bean->set(testHelper.p_uint, (Json::UInt)1, saveAtOnce);
    bean->set(testHelper.p_int64, (Json::Int64)-1, saveAtOnce);
    bean->set(testHelper.p_uint64, (Json::UInt64)1, saveAtOnce);
    bean->set(testHelper.p_real, (double)1.0, saveAtOnce);
    bean->set(testHelper.p_bool_0, false, saveAtOnce);
    bean->set(testHelper.p_bool_1, true, saveAtOnce);
    bean->set(testHelper.p_str, "foo", saveAtOnce);

    bean->addArrayProperty(testHelper.p_array_int);
    bean->append(testHelper.p_array_int, 101);
    bean->append(testHelper.p_array_int, 102);

    bean->addArrayProperty(testHelper.p_array_uint);
    bean->append(testHelper.p_array_uint, (Json::UInt)101);
    bean->append(testHelper.p_array_uint, (Json::UInt)102);

    bean->addArrayProperty(testHelper.p_array_real);
    bean->append(testHelper.p_array_real, (double)555.555);
    bean->append(testHelper.p_array_real, (double)888.888);

    bean->addArrayProperty(testHelper.p_array_bool);
    bean->append(testHelper.p_array_bool, true);
    bean->append(testHelper.p_array_bool, false);

    bean->addArrayProperty(testHelper.p_array_str);
    bean->append(testHelper.p_array_str, "str0");
    bean->append(testHelper.p_array_str, "str1");
}


void validateBeanProperties(TestHelper& testHelper, Bean* bean)
{
    EXPECT_TRUE(bean->get(testHelper.p_int).asInt() == -1);
    EXPECT_TRUE(bean->get(testHelper.p_uint).asUInt() == 1);
    EXPECT_TRUE(bean->get(testHelper.p_int64).asInt64() == (Json::Int64)-1);
    EXPECT_TRUE(bean->get(testHelper.p_uint64).asUInt64() == (Json::UInt64)1);
    EXPECT_TRUE(bean->get(testHelper.p_real).asDouble() == 1.0);
    EXPECT_TRUE(bean->get(testHelper.p_bool_0).asBool() == false);
    EXPECT_TRUE(bean->get(testHelper.p_bool_1).asBool() == true);
    EXPECT_TRUE(bean->get(testHelper.p_str).asString() == "foo");

    EXPECT_TRUE(bean->getAt(testHelper.p_array_int, 0).asInt() == 101);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_int, 1).asInt() == 102);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_uint, 0).asUInt() ==  (Json::UInt)101);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_uint, 1).asUInt() ==  (Json::UInt)102);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_real, 0).asDouble() == 555.555);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_real, 1).asDouble() == 888.888);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_bool, 0).asBool() == true);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_bool, 1).asBool() == false);
    EXPECT_TRUE(bean->getAt(testHelper.p_array_str, 0).asString() == "str0");
    EXPECT_TRUE(bean->getAt(testHelper.p_array_str, 1).asString() == "str1");

}


void setBeanNativeData(TestHelper& testHelper, Bean* bean, bool saveAtOnce)
{
    Json::Value nativeData;
    nativeData["root"]["p1"] = 1;
    nativeData["root"]["p2"] = 2;
    bean->setNativeData(nativeData, saveAtOnce);
}


void validateBeanNativeData(TestHelper& testHelper, Bean* bean)
{
    Json::Value nativeData;
    nativeData = bean->getNativeData();
    EXPECT_TRUE(nativeData["root"]["p1"]== 1);
    EXPECT_TRUE(nativeData["root"]["p2"]== 2);
}


void validateBean(TestHelper& testHelper, Bean* bean)
{
    validateBeanProperties(testHelper,  bean);
    validateBeanNativeData(testHelper, bean);
}


void test_find_init(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_bool_0, false);
    bean1->set(testHelper.p_bool_1, true);
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 2U);
    bean1->set(testHelper.p_int64, 3);
    bean1->set(testHelper.p_uint64, 4U);

    bean2->set(testHelper.p_real, 1.0);
    bean2->set(testHelper.p_str, "hello");
    bean2->set(testHelper.p_bool_0, false);
    bean2->set(testHelper.p_bool_1, true);
    bean2->set(testHelper.p_int, 1);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 3);
    bean2->set(testHelper.p_uint64, 4U);
}


void test_findEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2)
{
    std::list<oidType> beans;

    test_find_init(testHelper, world, bean1, bean2, nullptr);

    testHelper.p_real->findEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) == 1.0);
    }

    testHelper.p_bool_0->findEqual(false, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_bool_0) == false);
    }

    testHelper.p_bool_1->findEqual(true, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_bool_1) == true);
    }

    testHelper.p_str->findEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str) == "hello");
    }

   testHelper.p_int->findEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) == 1);
    }

    testHelper.p_uint->findEqual((uint_t)2U, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) == 2U);
    }
}


void test_findLessEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 1U);
    bean1->set(testHelper.p_int64, 101);
    bean1->set(testHelper.p_uint64, 101U);

    bean2->set(testHelper.p_real, 2.0);
    bean2->set(testHelper.p_str, "my");
    bean2->set(testHelper.p_int, 2);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 102);
    bean2->set(testHelper.p_uint64, 102U);

    bean3->set(testHelper.p_real, 3.0);
    bean3->set(testHelper.p_str, "world");
    bean3->set(testHelper.p_int, 3);
    bean3->set(testHelper.p_uint, 3U);
    bean3->set(testHelper.p_int64, 103);
    bean3->set(testHelper.p_uint64, 103U);


    testHelper.p_real->findLessEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) <= 1.0);
    }
    testHelper.p_real->findLessEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) <= 2.0);
    }
    testHelper.p_real->findLessEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) <= 3.0);
    }

    testHelper.p_str->findLessEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("hello") <= 0);
    }
    testHelper.p_str->findLessEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("my") <= 0);
    }
    testHelper.p_str->findLessEqual("world", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare( "world") <= 0);
    }

   testHelper.p_int->findLessEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) <= 1);
    }
   testHelper.p_int->findLessEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) <= 2);
    }
   testHelper.p_int->findLessEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) <= 3);
    }

    testHelper.p_uint->findLessEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) <= (uint_t)1);
    }
    testHelper.p_uint->findLessEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) <= (uint_t)2);
    }
    testHelper.p_uint->findLessEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) <= (uint_t)3);
    }
}


void test_FindGreaterEqual_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;
    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 1U);
    bean1->set(testHelper.p_int64, 101);
    bean1->set(testHelper.p_uint64, 101U);

    bean2->set(testHelper.p_real, 2.0);
    bean2->set(testHelper.p_str, "my");
    bean2->set(testHelper.p_int, 2);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 102);
    bean2->set(testHelper.p_uint64, 102U);

    bean3->set(testHelper.p_real, 3.0);
    bean3->set(testHelper.p_str, "world");
    bean3->set(testHelper.p_int, 3);
    bean3->set(testHelper.p_uint, 3U);
    bean3->set(testHelper.p_int64, 103);
    bean3->set(testHelper.p_uint64, 103U);

    testHelper.p_real->findGreaterEqual(1.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) >= 1.0);
    }
    testHelper.p_real->findGreaterEqual(2.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) >= 2.0);
    }
    testHelper.p_real->findGreaterEqual(3.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) >= 3.0);
    }

    testHelper.p_str->findGreaterEqual("hello", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("hello") >= 0);
    }
    testHelper.p_str->findGreaterEqual("my", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("my") >= 0);
    }
    testHelper.p_str->findGreaterEqual("world", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare( "world") >= 0);
    }

   testHelper.p_int->findGreaterEqual((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) >= 1);
    }
   testHelper.p_int->findGreaterEqual((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) >= 2);
    }
   testHelper.p_int->findGreaterEqual((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) >= 3);
    }

    testHelper.p_uint->findGreaterEqual((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) >= (uint_t)1);
    }
    testHelper.p_uint->findGreaterEqual((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) >= (uint_t)2);
    }
    testHelper.p_uint->findGreaterEqual((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) >= (uint_t)3);
    }
}


void test_findGreaterThan_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;

    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 1U);
    bean1->set(testHelper.p_int64, 101);
    bean1->set(testHelper.p_uint64, 101U);

    bean2->set(testHelper.p_real, 2.0);
    bean2->set(testHelper.p_str, "my");
    bean2->set(testHelper.p_int, 2);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 102);
    bean2->set(testHelper.p_uint64, 102U);

    bean3->set(testHelper.p_real, 3.0);
    bean3->set(testHelper.p_str, "world");
    bean3->set(testHelper.p_int, 3);
    bean3->set(testHelper.p_uint, 3U);
    bean3->set(testHelper.p_int64, 103);
    bean3->set(testHelper.p_uint64, 103U);


    testHelper.p_real->findGreaterThan(0.0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) > 0.0);
    }
    testHelper.p_real->findGreaterThan(1.0, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) > 1.0);
    }
    testHelper.p_real->findGreaterThan(2.0, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) > 2.0);
    }
    testHelper.p_real->findGreaterThan(3.0, beans);
    EXPECT_TRUE(beans.size() == 0);

    testHelper.p_str->findGreaterThan("a", beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("a") > 0);
    }
    testHelper.p_str->findGreaterThan("hello", beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("hello") > 0);
    }
    testHelper.p_str->findGreaterThan("my", beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("my") > 0);
    }
    testHelper.p_str->findGreaterThan("world", beans);
    EXPECT_TRUE(beans.size() == 0);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare( "world") > 0);
    }

    testHelper.p_int->findGreaterThan(0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) > 0);
    }

    testHelper.p_int->findGreaterThan(1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) > 1);
    }
    testHelper.p_int->findGreaterThan(2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) > 2);
    }
    testHelper.p_int->findGreaterThan(3, beans);
    EXPECT_TRUE(beans.size() == 0);

    testHelper.p_uint->findGreaterThan((uint_t)0, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) > (uint_t)0);
    }
    testHelper.p_uint->findGreaterThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) > (uint_t)1);
    }
    testHelper.p_uint->findGreaterThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) > (uint_t)2);
    }
    testHelper.p_uint->findGreaterThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 0);
}


void test_findLessThan_common(TestHelper& testHelper, BeanWorld& world, Bean* bean1, Bean* bean2, Bean* bean3)
{
    std::list<oidType> beans;

    bean1->set(testHelper.p_real, 1.0);
    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 1U);
    bean1->set(testHelper.p_int64, 101);
    bean1->set(testHelper.p_uint64, 101U);

    bean2->set(testHelper.p_real, 2.0);
    bean2->set(testHelper.p_str, "my");
    bean2->set(testHelper.p_int, 2);
    bean2->set(testHelper.p_uint, 2U);
    bean2->set(testHelper.p_int64, 102);
    bean2->set(testHelper.p_uint64, 102U);

    bean3->set(testHelper.p_real, 3.0);
    bean3->set(testHelper.p_str, "world");
    bean3->set(testHelper.p_int, 3);
    bean3->set(testHelper.p_uint, 3U);
    bean3->set(testHelper.p_int64, 103);
    bean3->set(testHelper.p_uint64, 103U);


    testHelper.p_real->findLessThan(1.0, beans);
   EXPECT_TRUE(beans.size() == 0);
    testHelper.p_real->findLessThan(2.0, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) < 2.0);
    }
    testHelper.p_real->findLessThan(3.0, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) < 3.0);
    }
    testHelper.p_real->findLessThan(4.0, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_real) < 4.0);
    }

    testHelper.p_str->findLessThan("hello", beans);
   EXPECT_TRUE(beans.size() == 0);
    testHelper.p_str->findLessThan("my", beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("my") < 0);
    }
    testHelper.p_str->findLessThan("world", beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare( "world") < 0);
    }
    testHelper.p_str->findLessThan("zzz", beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_str).compare("zzz") < 0);
    }

   testHelper.p_int->findLessThan((int_t)1, beans);
   EXPECT_TRUE(beans.size() == 0);
   testHelper.p_int->findLessThan((int_t)2, beans);
   EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) < 2);
    }
   testHelper.p_int->findLessThan((int_t)3, beans);
   EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) < 3);
    }
   testHelper.p_int->findLessThan(4, beans);
   EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_int) < 4);
    }

    testHelper.p_uint->findLessThan((uint_t)1, beans);
    EXPECT_TRUE(beans.size() == 0);
    testHelper.p_uint->findLessThan((uint_t)2, beans);
    EXPECT_TRUE(beans.size() == 1);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) < (uint_t)2);
    }
    testHelper.p_uint->findLessThan((uint_t)3, beans);
    EXPECT_TRUE(beans.size() == 2);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) < (uint_t)3);
    }
    testHelper.p_uint->findLessThan((uint_t)4, beans);
    EXPECT_TRUE(beans.size() == 3);
    for (auto& beanId : beans)
    {
        EXPECT_TRUE(world.getBean(beanId)->get(testHelper.p_uint) < (uint_t)4);
    }
}

void test_findObjects_common(TestHelper& testHelper, BeanWorld &world, Bean& bean1, Bean& bean2, Bean& bean3, std::list<oidType>& beans, bool needIndex)
{
    initTestHelper(testHelper, world, needIndex);

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

    bean1.addArrayRelation(testHelper.r_array_1);
    bean2.addArrayRelation(testHelper.r_array_1);
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

void test_findSubjects_common(TestHelper& testHelper, BeanWorld &world, Bean& bean1, Bean& bean2, Bean& bean3, std::list<oidType>& beans, bool needIndex)
{
    initTestHelper(testHelper, world, needIndex);

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

    bean1.addArrayRelation(testHelper.r_array_1);
    bean2.addArrayRelation(testHelper.r_array_1);
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

void test_getSubjects_common_array(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<oidType>& beans, bool needIndex)
{
    Property *pArray_1, *rArray_1;
   pArray_1 =  world.defineArrayProperty("p_array_int", Property::IntType);
   if (needIndex) pArray_1->createIndex();

    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.addArrayProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    pArray_1 = world.defineArrayProperty("p_array_int", Property::BoolType);
   if (needIndex) pArray_1->createIndex();

    bean1.addArrayProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(pArray_1);
    pArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

    rArray_1 = world.defineArrayRelation("r_array_1");
    if (needIndex) rArray_1->createIndex();

    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.addArrayRelation(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    bean1.removeProperty(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);
    bean1.addArrayRelation(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 1);
    world.undefineRelation(rArray_1);
    rArray_1->getSubjects(beans);
    EXPECT_TRUE(beans.size() == 0);

}

void test_getSubjects_common(BeanWorld &world, Bean& bean1, Bean& bean2, std::list<oidType>& beans, bool needIndex)
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

    bean1.set(testHelper.p_real, 1.0);
    bean1.set(testHelper.p_str, "hello");
    bean1.set(testHelper.p_bool_0, false);
    bean1.set(testHelper.p_bool_1, true);
    bean1.set(testHelper.p_int, 1);
    bean1.set(testHelper.p_uint, 2U);
    bean1.set(testHelper.p_int64, 3);
    bean1.set(testHelper.p_uint64, 4U);
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

    bean2.set(testHelper.p_real, 1.0);
    bean2.set(testHelper.p_str, "hello");
    bean2.set(testHelper.p_bool_0, false);
    bean2.set(testHelper.p_bool_1, true);
    bean2.set(testHelper.p_int, 1);
    bean2.set(testHelper.p_uint, 2U);
    bean2.set(testHelper.p_int64, 3);
    bean2.set(testHelper.p_uint64, 4U);
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