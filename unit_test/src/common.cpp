#include "./common.h"


////////////////////////////////////////////////////////
//global variables
////////////////////////////////////////////////////////
pidType pid_int_p = 0;
pidType pid_uint_p = 0;
pidType pid_int64_p = 0;
pidType pid_uint64_p = 0;
pidType pid_double_p = 0;
pidType pid_str_p = 0;
pidType pid_bool_p0 = 0;
pidType pid_bool_p1 = 0;
pidType pid_p1 = 0;
pidType pid_p2 = 0;
pidType pid_pArray_1 = 0;
pidType pid_r1 = 0;
pidType pid_rArray_1 =  0;

Property* double_p_property = nullptr;
Property* str_p_property = nullptr;
Property* int_p_property = nullptr;
Property* uint_p_property = nullptr;
Property* int64_p_property = nullptr;
Property* uint64_p_property = nullptr;
Property* bool_p0_property = nullptr;
Property* bool_p1_property = nullptr;
Property* p1_property = nullptr;
Property* p2_property = nullptr;
Property* pArray_1 = nullptr;
Property* r1 = nullptr;
Property* rArray_1 = nullptr;


void init_world(BeanWorld& world, bool needIndex)
{
    int_p_property = world.defineProperty("int_p", Property::IntType);
    uint_p_property = world.defineProperty("uint_p", Property::UIntType);
    int64_p_property = world.defineProperty("int64_p", Property::IntType);
    uint64_p_property = world.defineProperty("uint64_p", Property::UIntType);
    double_p_property = world.defineProperty("double_p", Property::RealType);
    str_p_property = world.defineProperty("str_p", Property::StringType);
    bool_p0_property = world.defineProperty("bool_p0", Property::BoolType);
    bool_p1_property = world.defineProperty("bool_p1", Property::BoolType);

    p1_property = world.defineProperty("p1", Property::IntType);
    p2_property = world.defineProperty("p2", Property::IntType);
    pArray_1 = world.defineArrayProperty("pArray_1", Property::IntType);
    r1 = world.defineRelation("r1");
    rArray_1 =  world.defineArrayRelation("rArray_1");

    if (needIndex)
    {
        double_p_property->createIndex();
        str_p_property->createIndex();
        int_p_property->createIndex();
        uint_p_property->createIndex();
        int64_p_property->createIndex();
        uint64_p_property->createIndex();
        bool_p0_property->createIndex();
        bool_p1_property->createIndex();
        p1_property->createIndex();
        p2_property->createIndex();
        pArray_1->createIndex();
        rArray_1->createIndex();
    }
}
