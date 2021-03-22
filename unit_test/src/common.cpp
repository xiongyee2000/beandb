#include "./common.h"


////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
Property* p_double = nullptr;
Property* p_str = nullptr;
Property* p_int = nullptr;
Property* p_uint = nullptr;
Property* p_int64 = nullptr;
Property* p_uint64 = nullptr;
Property* p_bool_0 = nullptr;
Property* p_bool_1 = nullptr;
Property* p1 = nullptr;
Property* p2 = nullptr;
Property* pArray_1 = nullptr;
Property* r1 = nullptr;
Property* r2 = nullptr;
Property* rArray_1 = nullptr;
Property* rArray_2 = nullptr;

void init_world(BeanWorld& world, bool needIndex)
{
    p_int = world.defineProperty("p_int", Property::IntType);
    p_uint = world.defineProperty("p_uint", Property::UIntType);
    p_int64 = world.defineProperty("p_int64", Property::IntType);
    p_uint64 = world.defineProperty("p_uint64", Property::UIntType);
    p_double = world.defineProperty("p_double", Property::RealType);
    p_str = world.defineProperty("p_str", Property::StringType);
    p_bool_0 = world.defineProperty("bool_p0", Property::BoolType);
    p_bool_1 = world.defineProperty("bool_p1", Property::BoolType);

    p1 = world.defineProperty("p1", Property::IntType);
    p2 = world.defineProperty("p2", Property::IntType);
    pArray_1 = world.defineArrayProperty("pArray_1", Property::IntType);
    r1 = world.defineRelation("r1");
    r2 = world.defineRelation("r2");
    rArray_1 =  world.defineArrayRelation("rArray_1");
    rArray_2 =  world.defineArrayRelation("rArray_2");

    if (needIndex)
    {
        p_double->createIndex();
        p_str->createIndex();
        p_int->createIndex();
        p_uint->createIndex();
        p_int64->createIndex();
        p_uint64->createIndex();
        p_bool_0->createIndex();
        p_bool_1->createIndex();
        p1->createIndex();
        p2->createIndex();
        pArray_1->createIndex();
        r1->createIndex();
        r2->createIndex();
        rArray_1->createIndex();
        rArray_2->createIndex();
    }
}
