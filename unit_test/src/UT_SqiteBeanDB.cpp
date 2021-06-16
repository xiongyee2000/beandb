

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

    testdbDir = "./unit_test/data/sqlite_db_dir";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}


TEST(SqliteBeanDB, saveProperty)
{
    Property *property;
    const char* testdbDir = "./unit_test/data/sqlite_db_dir";
    SqliteBeanDB testdb(testdbDir);
    BeanWorld world(&testdb);

    testdb.reInit();

    testdb.connect();

    property = world.defineProperty("p1", Property::IntType);
    testdb.saveProperty(property);

    property = world.defineProperty("p2", Property::UIntType);
    testdb.saveProperty(property);

    property = world.defineProperty("p3", Property::RealType);
    testdb.saveProperty(property);

    property = world.defineProperty("p4", Property::StringType);
    testdb.saveProperty(property);

    property = world.defineProperty("p5", Property::BoolType);
    testdb.saveProperty(property);

    world.clear();
    testdb.loadProperties();
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

    testdb.disconnect();

}