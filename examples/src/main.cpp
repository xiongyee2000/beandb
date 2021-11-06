#include <stdio.h>
#include "org/jinsha/bean/SqliteBeanDB.h"

using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./examples/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./examples/data/sqlite_db_1";

typedef struct TestHelper
{
    Property* p_real;
    Property* p_str;
    Property* p_int;
    Property* p_uint;
    Property* p_int64;
    Property* p_uint64;
    Property* p_bool_0;
    Property* p_bool_1;
    Property* p1;
    Property* p2;
    Property* p_array_int;
    Property* p_array_uint;
    Property* p_array_real;
    Property* p_array_bool;
    Property* p_array_str;
    Property* r1;
    Property* r2;
    Property* r_array_1;
    Property* r_array_2;
} TestHelper;


void initTestHelper(TestHelper& testHelper, BeanWorld* world)
{
    testHelper.p_int = world->defineProperty("p_int", Property::IntType);
    testHelper.p_uint = world->defineProperty("p_uint", Property::UIntType);
    testHelper.p_int64 = world->defineProperty("p_int64", Property::IntType);
    testHelper.p_uint64 = world->defineProperty("p_uint64", Property::UIntType);
    testHelper.p_real = world->defineProperty("p_real", Property::RealType);
    testHelper.p_str = world->defineProperty("p_str", Property::StringType);
    testHelper.p_bool_0 = world->defineProperty("p_bool_0", Property::BoolType);
    testHelper.p_bool_1 = world->defineProperty("p_bool_1", Property::BoolType);

    testHelper.p1 = world->defineProperty("p1", Property::IntType);
    testHelper.p2 = world->defineProperty("p2", Property::IntType);
    testHelper.p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    testHelper.p_array_uint = world->defineArrayProperty("p_array_uint", Property::UIntType);
    testHelper.p_array_real = world->defineArrayProperty("p_array_real", Property::RealType);
    testHelper.p_array_bool = world->defineArrayProperty("p_array_bool", Property::BoolType);
    testHelper.p_array_str = world->defineArrayProperty("p_array_str", Property::StringType);
    testHelper.r1 = world->defineRelation("r1");
    testHelper.r2 = world->defineRelation("r2");
    testHelper.r_array_1 =  world->defineArrayRelation("r_array_1");
    testHelper.r_array_2 =  world->defineArrayRelation("r_array_2");
}


void find_beanInit(TestHelper& testHelper, Bean* bean1, Bean* bean2, Bean* bean3)
{
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
}


void example_beandb_connect_disconnect()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;

    world = db.getWorld();
    if (world->getProperties().size() == 0)
        printf("No properties loaded for db is not connected. \n");
    else 
        printf("error occurred \n");

    err = db.connect();
    if (err) return;

    printf("db connected \n");
    if (world->getProperties().size() > 0)
        printf("%d properties loaded automatically \n", world->getProperties().size() );
    else 
        printf("error occurred \n");

    db.disconnect();
    printf("db disconnected \n");
    if (world->getProperties().size() == 0)
        printf("properties unloaded automatically \n" );
    else 
        printf("error occurred \n");

}


void example_world_defineProperty()
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

    db.disconnect();

}


void example_world_undefineProperty()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    oidType beanId_1 = 0;
    oidType beanId_2 = 0;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_int = world->defineProperty("p_int", Property::IntType);
    bean1 = world->newBean();
    bean2 = world->newBean();
    beanId_1 = bean1->getId();
    beanId_2 = bean2->getId();
    bean1->set(p_int, 1);
    printf("Bean1 property set: property name=\"%s\", value=%d \n", p_int->getName().c_str(), bean1->get(p_int).asInt());
    bean2->set(p_int, 2);
    printf("Bean2 property set: property name=\"%s\", value=%d \n", p_int->getName().c_str(), bean2->get(p_int).asInt());

    world->undefineProperty(p_int);
    printf("property p_int undefined \n");
    if (!bean1->isMember("p_int")) {
        printf("bean1 does not have property p_int now. \n");
    } else {
        printf("error occurred \n");
    }

    if (!bean2->isMember("p_int")) {
        printf("bean2 does not have property p_int now. \n");
    } else {
        printf("error occurred \n");
    }

    db.disconnect();
}

void example_world_newBean()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    bean = world->newBean();
    printf("Bean created: id=%d \n", bean->getId());

    db.disconnect();
}

void example_bean_set()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_int = world->defineProperty("p_int", Property::IntType);
    bean = world->newBean();
    bean->set(p_int, 1);
    printf("Bean property set: property name=\"%s\", value=%d \n", p_int->getName().c_str(), bean->get(p_int).asInt());

    db.disconnect();
}

void example_bean_append()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    bean = world->newBean();
    bean->addArray(p_array_int);
    printf("array property \"%s\" added for bean (id=%d, value=%d) \n", p_array_int->getName().c_str(), bean->getId());

    bean->append(p_array_int, 0);
    printf("value %d appended to bean's array property (name=\"%s\") \n", bean->getAt(p_array_int, 0).asInt(), p_array_int->getName().c_str());
    bean->append(p_array_int, 1);
    printf("value %d appended to bean's array property (name=\"%s\") \n", bean->getAt(p_array_int, 1).asInt(), p_array_int->getName().c_str());    

    db.disconnect();
}

void example_bean_setAt()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    bean = world->newBean();
    bean->addArray(p_array_int);
    printf("array property \"%s\" added for bean (id=%d, value=%d) \n", p_array_int->getName().c_str(), bean->getId());

    bean->append(p_array_int, 0);
    bean->append(p_array_int, 1);

    bean->setAt(p_array_int, 0, 100);
    printf("value %d set to bean's array property (name=\"%s\") at %d \n", bean->getAt(p_array_int, 0).asInt(), p_array_int->getName().c_str(), 0);
    bean->setAt(p_array_int, 1, 101);
    printf("value %d set to bean's array property (name=\"%s\") at %d \n", bean->getAt(p_array_int, 1).asInt(), p_array_int->getName().c_str(), 1);

    db.disconnect();
}

void example_bean_setRelation()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean *bean1, *bean2;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_r1 = world->defineRelation("r1");
    bean1 = world->newBean();
    bean2 = world->newBean();
    bean1->setRelation(p_r1, bean2);
    printf("Relation created: subjectId=%d, relation=\"%s\", objectId=%d \n", bean1->getId(), p_r1->getName().c_str(), bean1->getObjectId(p_r1));

    db.disconnect();
}

void example_bean_appendRelation()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean *bean1, *bean2, *bean3;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* r_array_1 = world->defineArrayRelation("r_array_1");
    bean1 = world->newBean();
    bean2 = world->newBean();
    bean3 = world->newBean();
    bean1->addArray(r_array_1);
    bean1->appendRelation(r_array_1, bean2);
    bean1->appendRelation(r_array_1, bean3);
    printf("Bean's (id=%d) relations (name=\"%s\"): \n", bean1->getId(), r_array_1->getName().c_str());
    printf("objectId=%d \n", bean1->getObjectId(r_array_1, 0));
    printf("objectId=%d \n", bean1->getObjectId(r_array_1, 1));

    db.disconnect();
}

void example_bean_setRelationAt()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean *bean1, *bean2, *bean3;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* r_array_1 = world->defineArrayRelation("r_array_1");
    bean1 = world->newBean();
    bean2 = world->newBean();
    bean3 = world->newBean();
    bean1->addArray(r_array_1);
    bean1->appendRelation(r_array_1, bean2);
    bean1->appendRelation(r_array_1, bean3);

    bean1->setRelationAt(r_array_1, 0, bean1);
    bean1->setRelationAt(r_array_1, 1, bean1);
    printf("Bean's (id=%d) relations (name=\"%s\"): \n", bean1->getId(), r_array_1->getName().c_str());
    printf("objectId=%d \n", bean1->getObjectId(r_array_1, 0));
    printf("objectId=%d \n", bean1->getObjectId(r_array_1, 1));

    db.disconnect();
}

void example_bean_setNativeData()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;
    Json::Value nativeData;

    nativeData["root"] = Json::Value(Json::objectValue);
    nativeData["root"]["np1"] = 1;
    nativeData["root"]["np2"] = "dummy";

    db.reInit();
    db.connect();
    world = db.getWorld();

    bean = world->newBean();
    bean->setNativeData(nativeData);
    nativeData = Json::Value::nullRef;
    nativeData = bean->getNativeData();

    printf("Bean's nativeData: \"%s\" \n", nativeData.toStyledString().c_str());

    db.disconnect();
}

void example_bean_save()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_int = world->defineProperty("p_int", Property::IntType);
    Property* p_bool = world->defineProperty("p_bool", Property::BoolType);
    Property* p_str = world->defineProperty("p_str", Property::StringType);
    bean = world->newBean();
    beanId = bean->getId();
    bean->set(p_int, 1, false);
    bean->set(p_bool, true, false);
    bean->set(p_str, "str", false);
    printf("Bean property set but not saved \n");
    db.disconnect();

    db.connect();
    p_int = world->getProperty("p_int");
    p_bool = world->getProperty("p_bool");
    p_str = world->getProperty("p_str");
    bean = world->getBean(beanId);
    if (bean->get(p_int).isNull()) {
        printf("Bean's property (name=\"%s\") has not been saved before. \n", p_int->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (bean->get(p_bool).isNull()) {
        printf("Bean's property (name=\"%s\") has not been saved before. \n", p_bool->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (bean->get(p_str).isNull()) {
        printf("Bean's property (name=\"%s\") has not been saved before. \n", p_str->getName().c_str());
    } else {
        printf("error occurred \n");
    }

    bean->set(p_int, 1, false);
    bean->set(p_bool, true, false);
    bean->set(p_str, "str", false);
    bean->save();
    printf("Bean property values saved \n");
    db.disconnect();

    db.connect();
    p_int = world->getProperty("p_int");
    p_bool = world->getProperty("p_bool");
    p_str = world->getProperty("p_str");
    bean = world->getBean(beanId);
    if (!bean->get(p_int).isNull()) {
        printf("Bean's property: name=\"%s\" value=%d . \n", p_int->getName().c_str(), bean->get(p_int).asInt());
    } else {
        printf("error occurred \n");
    }
    if (!bean->get(p_bool).isNull()) {
        printf("Bean's property: name=\"%s\" value=%d . \n", p_bool->getName().c_str(), bean->get(p_bool).asBool()  ? 1 : 0);
    } else {
        printf("error occurred \n");
    }
    if (!bean->get(p_str).isNull()) {
        printf("Bean's property: name=\"%s\" value=\"%s\" . \n", p_str->getName().c_str(), bean->get(p_str).asCString());
    } else {
        printf("error occurred \n");
    }

    db.disconnect();
}


void example_bean_remove()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_int = world->defineProperty("p_int", Property::IntType);
    Property* r1 = world->defineRelation("r1");
    Property* p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    Property* r_array = world->defineArrayRelation("r_array");

    bean1 = world->newBean();
    bean1->set(p_int, 1);
    printf("Bean property set: property name=\"%s\", value=%d \n", p_int->getName().c_str(), bean1->get(p_int).asInt());
    bean1->remove(p_int);
    if (bean1->get(p_int).isNull())
        printf("Bean property (name=\"%s\") removed. \n", p_int->getName().c_str());
    else 
        printf("error occurred\n");

    bean1->addArray(p_array_int);
    bean1->append(p_array_int, 0);
    bean1->append(p_array_int, 1);
    printf("Bean array property (name=\"%s\") has %d elements. \n", p_array_int->getName().c_str(), bean1->size(p_array_int));
    bean1->remove(p_array_int);
    if (bean1->size(p_array_int) == 0) 
        printf("Bean array property (name=\"%s\") removed. \n", p_array_int->getName().c_str());
    else
        printf("error occurred\n");

    bean2 = world->newBean();
    bean3 = world->newBean();
    bean1->setRelation(r1, bean2);
    printf("Bean relation set: relation name=\"%s\", objectBeanId=%d \n", r1->getName().c_str(), bean1->getObjectId(r1));
    bean1->remove(r1);
    if (bean1->getObjectId(r1) == 0)
        printf("Bean relation (name=\"%s\") removed. \n", r1->getName().c_str());
    else
        printf("error occurred\n");

    bean1->addArray(r_array);
    bean1->appendRelation(r_array, bean2);
    bean1->appendRelation(r_array, bean3);
    printf("Bean array relation (name=\"%s\") has %d elements. \n", r_array->getName().c_str(), bean1->size(r_array));
    bean1->remove(r_array);
    if (bean1->size(r_array) == 0) 
        printf("Bean array relation (name=\"%s\") removed. \n", r_array->getName().c_str());
    else
        printf("error occurred\n");

    db.disconnect();
}



void example_bean_removeAt()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean1 = nullptr;
    Bean* bean2 = nullptr;
    Bean* bean3 = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();

    Property* p_array_int = world->defineArrayProperty("p_array_int", Property::IntType);
    Property* r_array = world->defineArrayRelation("r_array");

    bean1 = world->newBean();
    bean1->addArray(p_array_int);
    bean1->append(p_array_int, 0);
    bean1->append(p_array_int, 1);
    printf("Bean array property (name=\"%s\") has %d elements. \n", p_array_int->getName().c_str(), bean1->size(p_array_int));
    bean1->removeAt(p_array_int, 0);
    if (bean1->getAt(p_array_int, 0).asInt() == 1) 
        printf("Bean array property (name=\"%s\") removed at index 0. \n", p_array_int->getName().c_str());
    else
        printf("error occurred\n");

    bean2 = world->newBean();
    bean3 = world->newBean();

    bean1->addArray(r_array);
    bean1->appendRelation(r_array, bean2);
    bean1->appendRelation(r_array, bean3);
    printf("Bean array relation (name=\"%s\") has %d elements. \n", r_array->getName().c_str(), bean1->size(r_array));
    bean1->removeAt(r_array, 0);
    if (bean1->getObjectId(r_array, 0) == bean3->getId()) 
        printf("Bean array relation (name=\"%s\") removed at index 0. \n", r_array->getName().c_str());
    else
        printf("error occurred\n");

    db.disconnect();
}


void example_bean_removeNativeData()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    Bean* bean = nullptr;
    Json::Value nativeData;

    nativeData["root"] = Json::Value(Json::objectValue);
    nativeData["root"]["np1"] = 1;
    nativeData["root"]["np2"] = "dummy";

    db.reInit();
    db.connect();
    world = db.getWorld();

    bean = world->newBean();
    bean->setNativeData(nativeData);
    nativeData = Json::Value::nullRef;
    nativeData = bean->getNativeData();

    printf("Bean's nativeData: \"%s\" \n", nativeData.toStyledString().c_str());

    bean->removeNativeData();
    nativeData = bean->getNativeData();
    if (nativeData.isNull())
        printf("native data removed \n"); 
    else
        printf("error occurred\n");

    db.disconnect();
}


void example_world_getBean()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();
    bean = world->getBean(999);
    if (bean == nullptr)
        printf("No such bean in db (beanId=%d) \n", 999);
    else 
        printf("error occurred \n"); 
    
    bean = world->getBean(1);
    if (bean != nullptr)
        printf("bean loaded from db (beanId=%d) \n", bean->getId());
    else 
        printf("error occurred \n"); 

    bean = world->getBean(1);
    if (bean != nullptr)
        printf("bean already loaded from db (beanId=%d) \n", bean->getId());
    else 
        printf("error occurred \n"); 

    db.disconnect();

}


void example_world_unloadBean()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();
     
    bean = world->getBean(1);
    if (bean != nullptr)
        printf("%d beans loaded \n", world->getCachedNumOfBeans());
    else 
        printf("error occurred \n"); 

    world->unloadBean(bean);
    if (world->getCachedNumOfBeans() == 0)
        printf("1 bean unloaded \n");
    else 
        printf("error occurred \n"); 

    bean = world->getBean(1);
    if (bean != nullptr)
        printf("bean loaded again from db \n");
    else 
        printf("error occurred \n"); 

    db.disconnect();

}


void example_world_deleteBean()
{
    char buff[128] = {0};
    char* cmd = &buff[0];
    sprintf(buff, "cp -rf %s/* %s/", g_sqlite_db_1, g_tmpDBDir);
    system(cmd);
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();
     
    bean = world->getBean(1);
    if (bean != nullptr)
        printf("%d beans loaded \n", world->getCachedNumOfBeans());
    else 
        printf("error occurred \n"); 

    world->deleteBean(bean);
    printf("deleteBean() called \n");
    if (world->getCachedNumOfBeans() == 0)
        printf("bean (id=1) removed from world \n");
    else 
        printf("error occurred \n"); 

    world->getBean(1);
    if (world->getCachedNumOfBeans() == 0)
        printf("bean (id=1) deleted from db \n");
    else 
        printf("error occurred \n"); 

    db.disconnect();

}


void example_world_unloadAllBeans()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();
     
    world->getBean(1);
    world->getBean(2);
    if (world->getCachedNumOfBeans() == 2)
        printf("2 beans loaded \n");
    else 
        printf("error occurred \n"); 

    world->unloadAllBeans();
    if (world->getCachedNumOfBeans() == 0)
        printf("2 beans unloaded \n");
    else 
        printf("error occurred \n"); 

    if (world->getProperties().size() > 0)
        printf("properties are still loaded \n");
    else 
        printf("error occurred \n"); 

    db.disconnect();

}


void example_world_unloadAll()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();
     
    world->getBean(1);
    world->getBean(2);
 
    world->unloadAll();
    if (world->getCachedNumOfBeans() == 0)
        printf("2 beans unloaded \n");
    else 
        printf("error occurred \n"); 

    if (world->getProperties().size() > 0)
        printf("properties unloaded \n");
    else 
        printf("error occurred \n"); 

    db.disconnect();

}


void example_world_loadAll()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();

    world->getBean(1);
    world->getBean(2);

    world->unloadAll();
    printf("2 beans unloaded \n");
 
    world->loadAll();
    printf("loadAll() called \n");
    if (world->getCachedNumOfBeans() > 0) {
        printf("Totally %d beans loaded \n", world->getCachedNumOfBeans());
        auto& beans = world->getCachedBeans();
        for (auto& item : beans) {
            printf("  bean: id=%d \n", item.first);
        }
    } else {
        printf("error occurred \n"); 
    }

    db.disconnect();

}


void example_page()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;
    Bean* bean = nullptr;
    oidType beanId = 0;

    db.connect();
    world = db.getWorld();

    BeanIdPage* page = world->getAllBeans(2);
    printf("world::getAllBeans() called with page size set to %d \n", page->getPageSize());
    if (page != nullptr) {
        printf("do forward traverse: \n");
        do {
            for (int i = 0; i < page->size(); i++) {
                printf("bean: id=%d \n", page->at(i));
            }
        } while (page->next() == 0);

        printf("do backward traverse: \n");
        do {
            for (int i = 0; i < page->size(); i++) {
                printf("bean: id=%d \n", page->at(i));
            }
        } while (page->prev() == 0);

        printf("goto first page...");
        err = page->gotoPage(0);
        if (err == 0) {
            printf("%d elements in the first page: \n", page->size());
            for (int i = 0; i < page->getPageSize(); i++) {
                printf("  bean: id=%d \n", page->at(i));
            }
        } else {
            printf("error occurred\n");
        }

        printf("goto 2nd page...");
        err = page->gotoPage(1);
        if (err == 0) {
            printf("%d elements in the 2nd page: \n", page->size());
            for (int i = 0; i < page->size(); i++) {
                printf("  bean: id=%d \n", page->at(i));
            }
        } else {
            printf("2nd page does not exist \n");
        }

        printf("goto 3rd page...");
        err = page->gotoPage(2);
        if (err == 0) {
            printf("%d elements in the 2n3rd page: \n", page->size());
            for (int i = 0; i < page->size(); i++) {
                printf("  bean: id=%d \n", page->at(i));
            }
        } else {
            printf("3rd page does not exist \n");
        }
    } else {
        printf("error occurred\n");
    }

    if (page != nullptr) delete page;

    db.disconnect();
}


void example_world_findEqual()
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();

    bean1->set(testHelper.p_str, "hello");
    bean1->set(testHelper.p_bool_0, false);
    bean1->set(testHelper.p_int, 1);
    bean1->set(testHelper.p_uint, 2U);

    bean2->set(testHelper.p_str, "hello");
    bean2->set(testHelper.p_bool_0, false);
    bean2->set(testHelper.p_int, 1);
    bean2->set(testHelper.p_uint, 2U);

    page = world->findEqual(testHelper.p_str, "hello");
    for (size_t i = 0; i < page->size(); i++)
    {
        printf("bean: id=%d, property=%s, value=%s \n", 
            page->at(i),
            testHelper.p_str->getName().c_str(),
            world->getBean(page->at(i))->get(testHelper.p_str).asCString());
    }
    if (page != nullptr) delete page;

    page = world->findEqual(testHelper.p_bool_0, false);
    for (size_t i = 0; i < page->size(); i++)
    {
        printf("bean: id=%d, property=%s, value=%s \n", 
            page->at(i),
            testHelper.p_bool_0->getName().c_str(),
            world->getBean(page->at(i))->get(testHelper.p_bool_0).asBool() ? "true" : "false");
    }
    if (page != nullptr) delete page;

    page = world->findEqual(testHelper.p_int, (int_t)1);
    for (size_t i = 0; i < page->size(); i++)
    {
        printf("bean: id=%d, property=%s, value=%d \n", 
            page->at(i),
            testHelper.p_int->getName().c_str(),
            world->getBean(page->at(i))->get(testHelper.p_int).asInt());
    }
    if (page != nullptr) delete page;

    page = world->findEqual(testHelper.p_uint, (uint_t)2U);
    for (size_t i = 0; i < page->size(); i++)
    {
        printf("bean: id=%d, property=%s, value=%u \n", 
            page->at(i),
            testHelper.p_uint->getName().c_str(),
            world->getBean(page->at(i))->get(testHelper.p_uint).asUInt());
    }
    if (page != nullptr) delete page;

    testdb.disconnect();
}


void example_world_findGreaterEqual()
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    find_beanInit(testHelper, bean1, bean2, bean3);

    page = world->findGreaterEqual(testHelper.p_real, 0.0);
    if (page != nullptr && page->size() == 3) {
        printf("There are %d bean(s) whose property (name=%s) value is greater equal to 0.0. \n", 
            page->size(), testHelper.p_real->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    page = world->findGreaterEqual(testHelper.p_real, 1.0);
    if (page != nullptr && page->size() == 3) {
        printf("There are %d bean(s) whose property (name=%s) value is greater equal to 1.0. \n", 
            page->size(), testHelper.p_real->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    page = world->findGreaterEqual(testHelper.p_real, 2.0);
    if (page != nullptr && page->size() == 2) {
        printf("There are %d bean(s) whose property (name=%s) value is greater equal to 2.0. \n", 
            page->size(), testHelper.p_real->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    page = world->findGreaterEqual(testHelper.p_real, 3.0);
    if (page != nullptr && page->size() == 1) {
        printf("There are %d bean(s) whose property (name=%s) value is greater equal to 3.0. \n", 
            page->size(), testHelper.p_real->getName().c_str());
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    testdb.disconnect();
}


void example_world_findLike()
{
    SqliteBeanDB db(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    db.reInit();
    db.connect();
    world = db.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    oidType beanId_1 = bean1->getId();
    Bean* bean2 = world->newBean();
    oidType beanId_2= bean2->getId();
    Bean* bean3 = world->newBean();
    oidType beanId_3 = bean3->getId();

    bean1->set(testHelper.p_str, "beandb");
    bean2->set(testHelper.p_str, "is");
    bean3->set(testHelper.p_str, "amazing!");

    bean1->addArray(testHelper.p_array_str);
    bean2->addArray(testHelper.p_array_str);
    bean3->addArray(testHelper.p_array_str);

    bean1->append(testHelper.p_array_str, "bean1 #1");
    bean1->append(testHelper.p_array_str, "bean1 #2");
    bean2->append(testHelper.p_array_str, "bean2 #1");
    bean2->append(testHelper.p_array_str, "bean2 #2");
    bean3->append(testHelper.p_array_str, "bean3 #1");
    bean3->append(testHelper.p_array_str, "bean3 #2");

    page = world->findLike(testHelper.p_str, "%a%");
    if (page != nullptr && page->size() > 0) {
        for (size_t i = 0; i < page->size(); i++)
        {
            printf("bean: id=%d, property=%s, value=%s \n", page->at(i), 
                testHelper.p_str->getName().c_str(),  
                world->getBean(page->at(i))->get(testHelper.p_str).asCString());
        }
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    page = world->findLike(testHelper.p_array_str, "bean1%");
    if (page != nullptr && page->size() > 0) {
        for (size_t i = 0; i < page->size(); i++)
        {
            printf("bean: id=%d, property=%s \n", page->at(i), 
                testHelper.p_str->getName().c_str());
        }
    } else {
        printf("error occurred \n");
    }
    if (page != nullptr) delete page;

    db.disconnect();
}


void example_findEqual_relation()
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();
    oidType beanId_1 = bean1->getId();
    oidType beanId_2 = bean2->getId();
    oidType beanId_3 = bean3->getId();

    bean1->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    bean3->setRelation(testHelper.r1, bean3);
    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    bean3->addArray(testHelper.r_array_1);
    bean1->appendRelation(testHelper.r_array_1, bean1);
    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean1->appendRelation(testHelper.r_array_1, bean3);
    bean2->appendRelation(testHelper.r_array_1, bean1);
    bean2->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean3);
    bean3->appendRelation(testHelper.r_array_1, bean1);
    bean3->appendRelation(testHelper.r_array_1, bean2);
    bean3->appendRelation(testHelper.r_array_1, bean3);

    page = world->findEqual(testHelper.r1, bean3->getId());
    if (page != nullptr) {
        printf("All beans that are subjects of bean3 on relation (name=%s): \n", 
            testHelper.r1->getName().c_str());
        do {
            for (size_t i = 0; i < page->size(); i++) {
                printf("  bean: id=%d \n", page->at(i));
            } 
        } while (page->next() == 0);
        delete page;
    } else {
        printf("error occurred \n");
    }
 
    page = world->findEqual(testHelper.r_array_1, bean1->getId());
    if (page != nullptr) {
        printf("All beans that are subjects of bean1 on relation (name=%s): \n", 
            testHelper.r_array_1->getName().c_str());
        do {
            for (size_t i = 0; i < page->size(); i++) {
                printf("  bean: id=%d \n", page->at(i));
            } 
        } while (page->next() == 0);
         delete page;
    } else {
        printf("error occurred \n");
    }
   
    testdb.disconnect();
}


void example_findSubjects()
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    bean1->setRelation(testHelper.r1, bean3);
    bean2->setRelation(testHelper.r1, bean3);
    page = world->findSubjects(testHelper.r1);
    if (page != nullptr && page->size() == 2) {
        printf("property r1 has %d subjects: \n", page->size());
        printf("  bean: id=%d \n", page->at(0));
        printf("  bean: id=%d \n", page->at(1));
    } else {
        printf("error ocurred \n");
    }
    if (page != nullptr) delete page;

    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    bean1->appendRelation(testHelper.r_array_1, bean1);
    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean1);
    bean2->appendRelation(testHelper.r_array_1, bean2);
    page = world->findSubjects(testHelper.r_array_1);
    if (page != nullptr && page->size() == 2) {
        printf("property r_array_1 has %d subjects: \n", page->size());
        printf("  bean: id=%d \n", page->at(0));
        printf("  bean: id=%d \n", page->at(1));
    } else {
        printf("error ocurred \n");
    }
    if (page != nullptr) delete page;

    testdb.disconnect();
}


void example_world_findObjects()
{
    SqliteBeanDB testdb(g_tmpDBDir);
    BeanWorld *world = nullptr;
    TestHelper testHelper;
    int err = 0;
    Json::Value value;
    BeanIdPage* page = nullptr;

    testdb.reInit();
    testdb.connect();
    world = testdb.getWorld();
    initTestHelper(testHelper, world);

    Bean* bean1 = world->newBean();
    Bean* bean2 = world->newBean();
    Bean* bean3 = world->newBean();

    bean1->setRelation(testHelper.r1, bean1);
    bean2->setRelation(testHelper.r1, bean2);
    page = world->findObjects(testHelper.r1);
    if (page != nullptr && page->size() == 2) {
        printf("property r1 has %d objects: \n", page->size());
        printf("  bean: id=%d \n", page->at(0));
        printf("  bean: id=%d \n", page->at(1));
    } else {
        printf("error ocurred \n");
    }
    if (page != nullptr) delete page;

    bean1->addArray(testHelper.r_array_1);
    bean2->addArray(testHelper.r_array_1);
    bean1->appendRelation(testHelper.r_array_1, bean1);
    bean1->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean2);
    bean2->appendRelation(testHelper.r_array_1, bean3);
    page = world->findObjects(testHelper.r_array_1);
    if (page != nullptr && page->size() == 3) {
        printf("property r_array_1 has %d objects: \n", page->size());
        printf("  bean: id=%d \n", page->at(0));
        printf("  bean: id=%d \n", page->at(1));
        printf("  bean: id=%d \n", page->at(2));
    } else {
        printf("error ocurred \n");
    }
    if (page != nullptr) delete page;
    
    testdb.disconnect();
}


int main(int argc, char* argv[])
{
    example_beandb_connect_disconnect();

    example_world_defineProperty();

    example_world_newBean();

    example_bean_set();

    example_bean_append();

    example_bean_setAt();

    example_bean_setRelation();

    example_bean_appendRelation();

    example_bean_setRelationAt();

    example_bean_setNativeData();

    example_bean_save();

    example_bean_remove();

    example_bean_removeAt();

    example_world_undefineProperty();

    example_bean_removeNativeData();

    example_world_getBean();

    example_world_unloadBean();

    example_world_deleteBean();

    example_world_unloadAllBeans();

    example_world_unloadAll();

    example_world_loadAll();

    example_page();

    example_world_findEqual();

    example_world_findGreaterEqual();

    example_world_findLike();

    example_findEqual_relation();

    example_findSubjects();

    example_world_findObjects();

    return 0;
};