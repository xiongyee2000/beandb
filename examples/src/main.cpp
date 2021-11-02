#include <stdio.h>
#include "org/jinsha/bean/SqliteBeanDB.h"

using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./examples/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./examples/data/sqlite_db_1";


// typedef struct TestHelper
// {
//     Property* p_real;
//     Property* p_str;
//     Property* p_int;
//     Property* p_uint;
//     Property* p_int64;
//     Property* p_uint64;
//     Property* p_bool_0;
//     Property* p_bool_1;
//     Property* p1;
//     Property* p2;
//     Property* p_array_int;
//     Property* p_array_uint;
//     Property* p_array_real;
//     Property* p_array_bool;
//     Property* p_array_str;
//     Property* r1;
//     Property* r2;
//     Property* r_array_1;
//     Property* r_array_2;
// } TestHelper;


// void initTestHelper(TestHelper& testHelper, BeanWorld& world)
// {
//     testHelper.p_int = world.defineProperty("p_int", Property::IntType);
//     testHelper.p_uint = world.defineProperty("p_uint", Property::UIntType);
//     testHelper.p_int64 = world.defineProperty("p_int64", Property::IntType);
//     testHelper.p_uint64 = world.defineProperty("p_uint64", Property::UIntType);
//     testHelper.p_real = world.defineProperty("p_real", Property::RealType);
//     testHelper.p_str = world.defineProperty("p_str", Property::StringType);
//     testHelper.p_bool_0 = world.defineProperty("p_bool_0", Property::BoolType);
//     testHelper.p_bool_1 = world.defineProperty("p_bool_1", Property::BoolType);

//     testHelper.p1 = world.defineProperty("p1", Property::IntType);
//     testHelper.p2 = world.defineProperty("p2", Property::IntType);
//     testHelper.p_array_int = world.defineArrayProperty("p_array_int", Property::IntType);
//     testHelper.p_array_uint = world.defineArrayProperty("p_array_uint", Property::UIntType);
//     testHelper.p_array_real = world.defineArrayProperty("p_array_real", Property::RealType);
//     testHelper.p_array_bool = world.defineArrayProperty("p_array_bool", Property::BoolType);
//     testHelper.p_array_str = world.defineArrayProperty("p_array_str", Property::StringType);
//     testHelper.r1 = world.defineRelation("r1");
//     testHelper.r2 = world.defineRelation("r2");
//     testHelper.r_array_1 =  world.defineArrayRelation("r_array_1");
//     testHelper.r_array_2 =  world.defineArrayRelation("r_array_2");
// }

void example_defineProperty()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_int = world->defineProperty("p_int", Property::IntType);
    printf("Property defined: id=%d, name=\"%s\", type=%d, valueType=%d \n", p_int->getId(), p_int->getName().c_str(), p_int->getType(), p_int->getValueType());

    Property* p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    printf("Property defined: id=%d, name=\"%s\", type=%d, valueType=%d \n", p_array_int->getId(), p_array_int->getName().c_str(), p_array_int->getType(), p_array_int->getValueType());

    Property* r1 = world->defineRelation("r1");
    printf("Relation defined: id=%d, name=\"%s\", type=%d, valueType=%d \n", r1->getId(), r1->getName().c_str(), r1->getType(), r1->getValueType());

    Property* r_array_1 =  world->defineArrayRelation("r_array_1");
    printf("Relation defined: id=%d, name=\"%s\", type=%d, valueType=%d \n", r_array_1->getId(), r_array_1->getName().c_str(), r_array_1->getType(), r_array_1->getValueType());

}

int main(int argc, char* argv[])
{
    example_defineProperty();

    return 0;
};