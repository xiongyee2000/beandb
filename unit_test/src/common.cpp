#include "./common.h"
#include <gmock/gmock.h>

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
    testHelper.rArray_2 =  world.defineArrayRelation("r_array_2");

    if (needIndex)
    {
//         testHelper.p_real->createIndex();
//         testHelper.p_str->createIndex();
//         testHelper.p_int->createIndex();
//         testHelper.p_uint->createIndex();
//         testHelper.p_int64->createIndex();
//         testHelper.p_uint64->createIndex();
//         testHelper.p_bool_0->createIndex();
//         testHelper.p_bool_1->createIndex();
//         testHelper.p1->createIndex();
//         testHelper.p2->createIndex();
//         testHelper.p_array_int->createIndex();
//         testHelper.r1->createIndex();
//         testHelper.r2->createIndex();
//         testHelper.r_array_1->createIndex();
//         testHelper.rArray_2->createIndex();
    }
}


void evaluate_testdb_empty_property(AbstractBeanDB& testdb)
{
    testdb.connect();
    std::unordered_map<std::string, Property*> propertyMap;
    testdb.loadProperties_(propertyMap);
    EXPECT_TRUE(propertyMap.size() == 0);
    testdb.disconnect();
}


void setBeanProperties(TestHelper& testHelper, Bean* bean)
{
    // bean->setProperty(testHelper.p_int, Json::Value::minInt);
    bean->setProperty(testHelper.p_int, -1);
    bean->setProperty(testHelper.p_uint, (Json::UInt)1);
    bean->setProperty(testHelper.p_int64, -1);
    bean->setProperty(testHelper.p_uint64, (Json::UInt64)1);
    bean->setProperty(testHelper.p_real, 1.0);
    bean->setProperty(testHelper.p_bool_0, false);
    bean->setProperty(testHelper.p_bool_1, true);
    bean->setProperty(testHelper.p_str, "foo");

    bean->createArrayProperty(testHelper.p_array_int);
    bean->appendProperty(testHelper.p_array_int, 101);
    bean->appendProperty(testHelper.p_array_int, 102);
}


void validateBeanProperties(TestHelper& testHelper, Bean* bean)
{
    EXPECT_TRUE(bean->getProperty(testHelper.p_int).asInt() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint).asUInt() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_int64).asInt64() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint64).asUInt64() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_real).asDouble() == 1.0);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_0).asBool() == false);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_1).asBool() == true);
    EXPECT_TRUE(bean->getProperty(testHelper.p_str).asString() == "foo");

    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 0).asInt() == 101);
    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 1).asInt() == 102);
}


void setBeanNativeData(TestHelper& testHelper, Bean* bean)
{
    Json::Value nativeData;
    nativeData["root"]["p1"] = 1;
    nativeData["root"]["p2"] = 2;
    bean->setNativeData(nativeData);
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
    EXPECT_TRUE(bean->getProperty(testHelper.p_int).asInt() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint).asUInt() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_int64).asInt64() == -1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_uint64).asUInt64() == 1);
    EXPECT_TRUE(bean->getProperty(testHelper.p_real).asDouble() == 1.0);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_0).asBool() == false);
    EXPECT_TRUE(bean->getProperty(testHelper.p_bool_1).asBool() == true);
    EXPECT_TRUE(bean->getProperty(testHelper.p_str).asString() == "foo");

    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 0).asInt() == 101);
    EXPECT_TRUE(bean->getArrayProperty(testHelper.p_array_int, 1).asInt() == 102);
}
