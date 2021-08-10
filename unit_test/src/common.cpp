#include "./common.h"


////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
// Property* p_double = nullptr;
// Property* p_str = nullptr;
// Property* p_int = nullptr;
// Property* p_uint = nullptr;
// Property* p_int64 = nullptr;
// Property* p_uint64 = nullptr;
// Property* p_bool_0 = nullptr;
// Property* p_bool_1 = nullptr;
// Property* p1 = nullptr;
// Property* p2 = nullptr;
// Property* pArray_1 = nullptr;
// Property* r1 = nullptr;
// Property* r2 = nullptr;
// Property* rArray_1 = nullptr;
// Property* rArray_2 = nullptr;

void init_world(TestHelper& testHelper, BeanWorld& world, bool needIndex)
{
    testHelper.p_int = world.defineProperty("p_int", Property::IntType);
    testHelper.p_uint = world.defineProperty("p_uint", Property::UIntType);
    testHelper.p_int64 = world.defineProperty("p_int64", Property::IntType);
    testHelper.p_uint64 = world.defineProperty("p_uint64", Property::UIntType);
    testHelper.p_double = world.defineProperty("p_double", Property::RealType);
    testHelper.p_str = world.defineProperty("p_str", Property::StringType);
    testHelper.p_bool_0 = world.defineProperty("bool_p0", Property::BoolType);
    testHelper.p_bool_1 = world.defineProperty("bool_p1", Property::BoolType);

    testHelper.p1 = world.defineProperty("p1", Property::IntType);
    testHelper.p2 = world.defineProperty("p2", Property::IntType);
    testHelper.pArray_1 = world.defineArrayProperty("pArray_1", Property::IntType);
    testHelper.r1 = world.defineRelation("r1");
    testHelper.r2 = world.defineRelation("r2");
    testHelper.rArray_1 =  world.defineArrayRelation("rArray_1");
    testHelper.rArray_2 =  world.defineArrayRelation("rArray_2");

    if (needIndex)
    {
        testHelper.p_double->createIndex();
        testHelper.p_str->createIndex();
        testHelper.p_int->createIndex();
        testHelper.p_uint->createIndex();
        testHelper.p_int64->createIndex();
        testHelper.p_uint64->createIndex();
        testHelper.p_bool_0->createIndex();
        testHelper.p_bool_1->createIndex();
        testHelper.p1->createIndex();
        testHelper.p2->createIndex();
        testHelper.pArray_1->createIndex();
        testHelper.r1->createIndex();
        testHelper.r2->createIndex();
        testHelper.rArray_1->createIndex();
        testHelper.rArray_2->createIndex();
    }
}
