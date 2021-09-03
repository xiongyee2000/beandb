#include "./common.h"


////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////

void init_world(TestHelper& testHelper, BeanWorld& world, bool needIndex)
{
    testHelper.p_int = world.defineProperty("p_int", Property::IntType);
    testHelper.p_uint = world.defineProperty("p_uint", Property::UIntType);
    testHelper.p_int64 = world.defineProperty("p_int64", Property::IntType);
    testHelper.p_uint64 = world.defineProperty("p_uint64", Property::UIntType);
    testHelper.p_real = world.defineProperty("p_double", Property::RealType);
    testHelper.p_str = world.defineProperty("p_str", Property::StringType);
    testHelper.p_bool_0 = world.defineProperty("bool_p0", Property::BoolType);
    testHelper.p_bool_1 = world.defineProperty("bool_p1", Property::BoolType);

    testHelper.p1 = world.defineProperty("p1", Property::IntType);
    testHelper.p2 = world.defineProperty("p2", Property::IntType);
    testHelper.p_array_int = world.defineArrayProperty("p_array_int", Property::IntType);
    testHelper.p_array_uint = world.defineArrayProperty("p_array_uint", Property::IntType);
    testHelper.p_array_real = world.defineArrayProperty("p_array_real", Property::IntType);
    testHelper.p_array_bool = world.defineArrayProperty("p_array_bool", Property::IntType);
    testHelper.p_array_str = world.defineArrayProperty("p_array_str", Property::IntType);
    testHelper.r1 = world.defineRelation("r1");
    testHelper.r2 = world.defineRelation("r2");
    testHelper.r_array_1 =  world.defineArrayRelation("r_array_1");
    testHelper.rArray_2 =  world.defineArrayRelation("r_array_2");

    if (needIndex)
    {
        // testHelper.p_real->createIndex();
        // testHelper.p_str->createIndex();
        // testHelper.p_int->createIndex();
        // testHelper.p_uint->createIndex();
        // testHelper.p_int64->createIndex();
        // testHelper.p_uint64->createIndex();
        // testHelper.p_bool_0->createIndex();
        // testHelper.p_bool_1->createIndex();
        // testHelper.p1->createIndex();
        // testHelper.p2->createIndex();
        // testHelper.p_array_int->createIndex();
        // testHelper.r1->createIndex();
        // testHelper.r2->createIndex();
        // testHelper.r_array_1->createIndex();
        // testHelper.rArray_2->createIndex();
    }
}
