

#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <string>

//This is used to test private methods
#define private public
//This is used to test protected methods
#define protected public

#include "org/jinsha/bean/SqliteBeanDB.h"

#include "./common.h"

using namespace std;
using namespace Json;
using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./unit_test/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./unit_test/data/sqlite_db_1";

static void validate_testdb_1(SqliteBeanDB& testdb);
static void evaluate_testdb_empty_property(SqliteBeanDB& testdb);

TEST(SqliteBeanDB, constuctor)
{
    BeanWorld world;
    const char* testdbDir = nullptr;
    SqliteBeanDB* testdb;
    
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = "";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = "invalid";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() != 0);
    delete testdb;

    testdbDir = g_tmpDBDir;
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}


TEST(SqliteBeanDB, connect_disconnect)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    int err = 0;

    err = testdb.connect();
    EXPECT_TRUE(err == -2);
    err = testdb.disconnect();
    EXPECT_TRUE(err == 0);

    BeanWorld world(&testdb);
    err = testdb.connect();
    EXPECT_TRUE(err == 0);
    err = testdb.disconnect();
    EXPECT_TRUE(err == 0);
}

TEST(SqliteBeanDB, getProperty)
{
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);
    testdb.connect();
   validate_testdb_1(testdb);
   testdb.disconnect();
}

TEST(SqliteBeanDB, loadProperties)
{
    // Property *property;
    const char* testdbDir = g_sqlite_db_1;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);
    int err = 0;

    world.clear();

    testdb.connect();

    err = testdb.loadProperties();
    EXPECT_TRUE(err == 0);

    EXPECT_TRUE(world.getProperty("p1")->getName() == "p1" && 
        world.getProperty("p1")->getType() == Property::PrimaryType && 
        world.getProperty("p1")->getValueType() == Property::IntType);
    EXPECT_TRUE(world.getProperty("p2")->getName() == "p2" && 
        world.getProperty("p2")->getType() == Property::PrimaryType && 
        world.getProperty("p2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world.getProperty("p3")->getName() == "p3" && 
        world.getProperty("p3")->getType() == Property::PrimaryType && 
        world.getProperty("p3")->getValueType() == Property::RealType);
    EXPECT_TRUE(world.getProperty("p4")->getName() == "p4" && 
        world.getProperty("p4")->getType() == Property::PrimaryType && 
        world.getProperty("p4")->getValueType() == Property::StringType);
    EXPECT_TRUE(world.getProperty("p5")->getName() == "p5" && 
        world.getProperty("p5")->getType() == Property::PrimaryType && 
        world.getProperty("p5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world.getProperty("ap1")->getName() == "ap1" && 
        world.getProperty("ap1")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap1")->getValueType() == Property::IntType);
    EXPECT_TRUE(world.getProperty("ap2")->getName() == "ap2" && 
        world.getProperty("ap2")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap2")->getValueType() == Property::UIntType);
    EXPECT_TRUE(world.getProperty("ap3")->getName() == "ap3" && 
        world.getProperty("ap3")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap3")->getValueType() == Property::RealType);
    EXPECT_TRUE(world.getProperty("ap4")->getName() == "ap4" && 
        world.getProperty("ap4")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap4")->getValueType() == Property::StringType);
    EXPECT_TRUE(world.getProperty("ap5")->getName() == "ap5" && 
        world.getProperty("ap5")->getType() == Property::ArrayPrimaryType && 
        world.getProperty("ap5")->getValueType() == Property::BoolType);

    EXPECT_TRUE(world.getProperty("r1")->getName() == "r1" && 
        world.getProperty("r1")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r2")->getName() == "r2" && 
        world.getProperty("r2")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r3")->getName() == "r3" && 
        world.getProperty("r3")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r4")->getName() == "r4" && 
        world.getProperty("r4")->getType() == Property::RelationType);
    EXPECT_TRUE(world.getProperty("r5")->getName() == "r5" && 
        world.getProperty("r5")->getType() == Property::RelationType);

    EXPECT_TRUE(world.getProperty("ar1")->getName() == "ar1" && 
        world.getProperty("ar1")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar2")->getName() == "ar2" && 
        world.getProperty("ar2")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar3")->getName() == "ar3" && 
        world.getProperty("ar3")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar4")->getName() == "ar4" && 
        world.getProperty("ar4")->getType() == Property::ArrayRelationType);
    EXPECT_TRUE(world.getProperty("ar5")->getName() == "ar5" && 
        world.getProperty("ar5")->getType() == Property::ArrayRelationType);
    
    testdb.disconnect();

}

TEST(SqliteBeanDB,createProperty_deleteProperty)
{
    Property *property = nullptr;
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);
    int err = 0;

    testdb.reInit();
    testdb.connect();

    property = testdb.createProperty("p1", Property::IntType);
    EXPECT_TRUE(property->getName() == "p1");
    property = testdb.createProperty("p2", Property::UIntType);
    EXPECT_TRUE(property->getName() == "p2");
    property = testdb.createProperty("p3", Property::RealType);
    EXPECT_TRUE(property->getName() == "p3");
    property = testdb.createProperty("p4", Property::StringType);
    EXPECT_TRUE(property->getName() == "p4");
    property = testdb.createProperty("p5", Property::BoolType);
    EXPECT_TRUE(property->getName() == "p5");
    property = testdb.createArrayProperty("ap1", Property::IntType);
    EXPECT_TRUE(property->getName() == "ap1");
    property = testdb.createArrayProperty("ap2", Property::UIntType);
    EXPECT_TRUE(property->getName() == "ap2");
    property = testdb.createArrayProperty("ap3", Property::RealType);
    EXPECT_TRUE(property->getName() == "ap3");
    property = testdb.createArrayProperty("ap4", Property::StringType);
    EXPECT_TRUE(property->getName() == "ap4");
    property = testdb.createArrayProperty("ap5", Property::BoolType);
    EXPECT_TRUE(property->getName() == "ap5");
    property = testdb.createRelation("r1");
    EXPECT_TRUE(property->getName() == "r1");
    property = testdb.createRelation("r2");
    EXPECT_TRUE(property->getName() == "r2");
    property = testdb.createRelation("r3");
    EXPECT_TRUE(property->getName() == "r3");
    property = testdb.createRelation("r4");
    EXPECT_TRUE(property->getName() == "r4");
    property = testdb.createRelation("r5");
    EXPECT_TRUE(property->getName() == "r5");
    property = testdb.createArrayRelation("ar1");
    EXPECT_TRUE(property->getName() == "ar1");
    property = testdb.createArrayRelation("ar2");
    EXPECT_TRUE(property->getName() == "ar2");
    property = testdb.createArrayRelation("ar3");
    EXPECT_TRUE(property->getName() == "ar3");
    property = testdb.createArrayRelation("ar4");
    EXPECT_TRUE(property->getName() == "ar4");
    property = testdb.createArrayRelation("ar5");
    EXPECT_TRUE(property->getName() == "ar5");

    validate_testdb_1(testdb);

    err = testdb.deleteProperty("p1");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("p2");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("p3");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("p4");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("p5");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ap1");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ap2");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ap3");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ap4");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ap5");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("r1");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("r2");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("r3");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("r4");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("r5");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ar1");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ar2");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ar3");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ar4");
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("ar5");
    EXPECT_TRUE(err == 0);

    evaluate_testdb_empty_property(testdb);

    err = testdb.deleteProperty(nullptr);
    EXPECT_TRUE(err == 0);
    err = testdb.deleteProperty("");
    EXPECT_TRUE(err == 0);

    testdb.clear();
    testdb.disconnect();

}


TEST(SqliteBeanDB,createBean_deleteBean)
{
    Bean *bean = nullptr;
    const char* testdbDir = g_tmpDBDir;
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);
    int err = 0;

    testdb.reInit();
    testdb.connect();

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    bean = testdb.createBean();
    EXPECT_TRUE(bean != nullptr);
    
    err = testdb.deleteBean(bean);
    EXPECT_TRUE(err == 0);

    err = testdb.deleteBean(nullptr);
    EXPECT_TRUE(err == 0);


    testdb.clear();
    testdb.disconnect();
}


static void validate_testdb_1(SqliteBeanDB& testdb)
{
    Property *property = nullptr;

    property = testdb.getProperty("p1");
    EXPECT_TRUE(testdb.getProperty("p1")->getName() == "p1" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::IntType);

    property = testdb.getProperty("p2");
    EXPECT_TRUE(property->getName() == "p2" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::UIntType);
    
    property = testdb.getProperty("p3");
    EXPECT_TRUE(property->getName() == "p3" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::RealType);
    
    property = testdb.getProperty("p4");
    EXPECT_TRUE(property->getName() == "p4" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::StringType);

    property = testdb.getProperty("p5");
    EXPECT_TRUE(property->getName() == "p5" && 
        property->getType() == Property::PrimaryType && 
        property->getValueType() == Property::BoolType);

    property = testdb.getProperty("ap1");
    EXPECT_TRUE(property->getName() == "ap1" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::IntType);

    property = testdb.getProperty("ap2");
    EXPECT_TRUE(property->getName() == "ap2" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::UIntType);

    property = testdb.getProperty("ap3");
    EXPECT_TRUE(property->getName() == "ap3" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::RealType);

    property = testdb.getProperty("ap4");
    EXPECT_TRUE(property->getName() == "ap4" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::StringType);

    property = testdb.getProperty("ap5");
    EXPECT_TRUE(property->getName() == "ap5" && 
        property->getType() == Property::ArrayPrimaryType && 
        property->getValueType() == Property::BoolType);

    property = testdb.getProperty("r1");
    EXPECT_TRUE(property->getName() == "r1" && 
        property->getType() == Property::RelationType);

    property = testdb.getProperty("r2");
    EXPECT_TRUE(property->getName() == "r2" && 
        property->getType() == Property::RelationType);

    property = testdb.getProperty("r3");
    EXPECT_TRUE(property->getName() == "r3" && 
        property->getType() == Property::RelationType);

    property = testdb.getProperty("r4");
    EXPECT_TRUE(property->getName() == "r4" && 
        property->getType() == Property::RelationType);
 
    property = testdb.getProperty("r5");
    EXPECT_TRUE(property->getName() == "r5" && 
        property->getType() == Property::RelationType);

    property = testdb.getProperty("ar1");
    EXPECT_TRUE(property->getName() == "ar1" && 
        property->getType() == Property::ArrayRelationType);

    property = testdb.getProperty("ar2");
    EXPECT_TRUE(property->getName() == "ar2" && 
        property->getType() == Property::ArrayRelationType);

    property = testdb.getProperty("ar3");
    EXPECT_TRUE(property->getName() == "ar3" && 
        property->getType() == Property::ArrayRelationType);

    property = testdb.getProperty("ar4");
    EXPECT_TRUE(property->getName() == "ar4" && 
        property->getType() == Property::ArrayRelationType);

    property = testdb.getProperty("ar5");
    EXPECT_TRUE(property->getName() == "ar5" && 
        property->getType() == Property::ArrayRelationType);

}


void evaluate_testdb_empty_property(SqliteBeanDB& testdb)
{
    testdb.connect();
    testdb.getWorld()->loadProperties();
    EXPECT_TRUE(testdb.getWorld()->getProperties().size() == 0);
    testdb.disconnect();
}