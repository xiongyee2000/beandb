# BeanDB

#### Description
BeanDB is a simple, small, and easy to use object database engine.

#### Application Domain
BeanDB is aimed to be used in those database-based applications running on "rich" devices, e.g.: vehicle side autonomous driving applications, or some applications running on IoT devices.

#### Development Language
BeanDB is written in C++.

#### Characteristics
BeanDB has following characteristics:

#### - Small
- lines of code < 3000
- binary size < 200k


#### - Object Oriented
In BeanDB, data are represented as object (called bean) and stored as object. This is by definition the main characteristic of object database.

#### - Dynamic Property
In BeanDB, properties are dynamic, meaning they can be dynamically added/removed/modified per user's need at runtime. This is also one of main characteristics of the object database.

#### - JSON like bean
The bean in BeanDB is built on top of JSON, and is designed to be JSON like. 

#### - Strong typed property
The property in BeanDB is strong typed. It’s mandatory to specify the value type, e.g. int/real/bool/string etc. when a property is defined. This is different from JSON.

#### - Global property
The property defined in a BeanDB is global. The property with the same name will have the same semantic meaning. Actually, this is the foundation of the property-based search.

#### - Native data support
In BeanDB, it is supported to attach arbitrary JSON data to a bean, which is call native data. This mechanism makes it possible to easily realize data persistence for JSON data in arbitrary structure via BeanDB.

#### Basic Features

- Define/Undefine properties (see more detail in the examples)
- New/Delete bean objects from the database (see more detail in the examples)
- Load/Unload bean objects from the database (see more detail in the examples)
- Add/Remove/Set property values (see more detail in the examples)
- Save bean/bean property value (see more detail in the examples)
- Property based search (see more detail in the examples)


#### Software Architecture
TBD

#### How to build
(Current build environment: Ubuntu 7.5)
##### Build dependencies
- gcc (c++11)
- cmake (>2.8)
- jsoncpp
- sqlite3

###### Build beandb libraries
```
./build.sh
```
（build output is located under ./dist）
<br>

##### Build examples (example programs)

```
./build_examples.sh
```
(build output is located under ./build_examples)


##### Build doxygen documenation（Need to install doxygen）
```
./doxy_gen.sh
```
(build output is located under ./ doc/doxygen/html/)


#### How to use
See example programs under ./examples.


#### Planned features in future
- JSON binary storage
- binary property (blob) support
- class definitions of bean objects
- Console query/control
- Transaction support
- Query language support
- More database backend implementation
- APIs in multiple languages
- Backup/Restore
- Synchronization Mechanism between device/cloud
- more…


#### Contributors
Tony: xiongyee@hotmail.com


#### LICENSE
GPL 3.0

 ** Welcome to join us! **  :smile:
