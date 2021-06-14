

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

    testdbDir = "./unit_test/data/db_dir";
    testdb = new SqliteBeanDB(testdbDir);
    EXPECT_TRUE(testdb->getDir() == testdbDir);
    EXPECT_TRUE(testdb->checkDB() == 0);
    delete testdb;
}


TEST(SqliteBeanDB, saveProperty)
{
    BeanWorld world;
    Property *property;
    const char* testdbDir = "./unit_test/data/db_dir";
    SqliteBeanDB testdb(testdbDir);

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

    testdb.disconnect();

}