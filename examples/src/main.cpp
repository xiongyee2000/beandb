#include <stdio.h>
#include "org/jinsha/bean/SqliteBeanDB.h"

using namespace org::jinsha::bean;

static const char* g_tmpDBDir = "./examples/data/sqlite_tmp_db";
static const char* g_sqlite_db_1 = "./examples/data/sqlite_db_1";


void example_connect_disconnect()
{
    SqliteBeanDB db(g_sqlite_db_1);
    BeanWorld* world = nullptr;
    int err = 0;

    world = db.getWorld();
    if (world->getProperties().size() == 0)
        printf("No properties loaded for db is not connected. \n");
    else 
        printf("error occured \n");

    err = db.connect();
    if (err) return;

    printf("db connected \n");
    if (world->getProperties().size() > 0)
        printf("%d properties loaded automatically \n", world->getProperties().size() );
    else 
        printf("error occured \n");

    db.disconnect();
    printf("db disconnected \n");
    if (world->getProperties().size() == 0)
        printf("properties unloaded automatically \n" );
    else 
        printf("error occured \n");

}


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

    db.disconnect();

}

void example_newBean()
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
        printf("error occured \n");
    }
    if (bean->get(p_bool).isNull()) {
        printf("Bean's property (name=\"%s\") has not been saved before. \n", p_bool->getName().c_str());
    } else {
        printf("error occured \n");
    }
    if (bean->get(p_str).isNull()) {
        printf("Bean's property (name=\"%s\") has not been saved before. \n", p_str->getName().c_str());
    } else {
        printf("error occured \n");
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
        printf("error occured \n");
    }
    if (!bean->get(p_bool).isNull()) {
        printf("Bean's property: name=\"%s\" value=%d . \n", p_bool->getName().c_str(), bean->get(p_bool).asBool()  ? 1 : 0);
    } else {
        printf("error occured \n");
    }
    if (!bean->get(p_str).isNull()) {
        printf("Bean's property: name=\"%s\" value=\"%s\" . \n", p_str->getName().c_str(), bean->get(p_str).asCString());
    } else {
        printf("error occured \n");
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
        printf("error occured\n");

    bean1->addArray(p_array_int);
    bean1->append(p_array_int, 0);
    bean1->append(p_array_int, 1);
    printf("Bean array property (name=\"%s\") has %d elements. \n", p_array_int->getName().c_str(), bean1->size(p_array_int));
    bean1->remove(p_array_int);
    if (bean1->size(p_array_int) == 0) 
        printf("Bean array property (name=\"%s\") removed. \n", p_array_int->getName().c_str());
    else
        printf("error occured\n");

    bean2 = world->newBean();
    bean3 = world->newBean();
    bean1->setRelation(r1, bean2);
    printf("Bean relation set: relation name=\"%s\", objectBeanId=%d \n", r1->getName().c_str(), bean1->getObjectId(r1));
    bean1->remove(r1);
    if (bean1->getObjectId(r1) == 0)
        printf("Bean relation (name=\"%s\") removed. \n", r1->getName().c_str());
    else
        printf("error occured\n");

    bean1->addArray(r_array);
    bean1->appendRelation(r_array, bean2);
    bean1->appendRelation(r_array, bean3);
    printf("Bean array relation (name=\"%s\") has %d elements. \n", r_array->getName().c_str(), bean1->size(r_array));
    bean1->remove(r_array);
    if (bean1->size(r_array) == 0) 
        printf("Bean array relation (name=\"%s\") removed. \n", r_array->getName().c_str());
    else
        printf("error occured\n");

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
        printf("error occured\n");

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
        printf("error occured\n");

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
        printf("error occured\n");

    db.disconnect();
}


int main(int argc, char* argv[])
{
    example_connect_disconnect();

    example_defineProperty();

    example_newBean();

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

    example_bean_removeNativeData();

    return 0;
};