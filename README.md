# bean

#### 介绍
BeanDB是一个小巧精炼的对象数据库。


#### 应用场景
BeanDB的最佳应用场景是嵌入式设备上基于数据库的应用场景，比如支持自动驾驶的智能汽车上的应用场景。


#### 开发语言
BeanDB采用C++语言开发。


#### 特点
BeanDB具有以下特点：
- 面向对象
在BeanDB中，数据被表示为对象（这样的对象叫做bean），对数据的存储也是以对象为单位。这是对象数据库的基本特点之一。
- 动态属性
在BeanDB中，bean的属性是运行时动态的，用户可以根据需要在运行时动态增加/删除/修改。这是对象数据库的基本特点之一。
- 类JSON对象
BeanDB中的bean对象采用JSON作为载体，是类JSON的，以此实现其属性动态性。
- 强类型属性
BeanDB中的属性是强类型的。定义属性的时候必须指明其类型，比如int/real/bool/string等。这点和JSON有所不同。
- 全局属性
BeanDB中的属性定义是全局的。不同对象的相同名字的属性意味着相同的语义。比如bean1的属性name与bean2的属性name一定对应于同一个预定义的全局属性。如果全局属性name被取消定义，bean1和bean2的name属性均会被删除。实际上，属性的全局性是可以基于该属性进行搜索的基础。
- 原生数据支持
BeanDB支持将任意JSON数据作为附件附加到一个bean对象上，称为原生数据。这种机制使得任意结构的JSON数据都可以通过BeanDB很方便地实现持久化。


#### 基本功能

- 定义全局属性/删除全局属性（详见示例程序）
- 从数据库创建对象/删除对象（详见示例程序）
- 从数据库加载对象（详见示例程序）
- 为对象添加/移除/设置属性（详见示例程序）
- 为对象添加/移除/设置原生数据（详见示例程序）
- 保存对象/对象属性（详见示例程序）
- 基于属性值的搜索（详见示例程序）


#### 软件架构
TBD


#### 编译构建
- 构建beandb库：
./build.sh
（构建结果将位于dist目录下）

- 构建examples（示例程序）：
./build_examples.sh
（构建结果将位于build_examples目录下）


#### 使用说明
详见examples目录下的示例程序。


#### 参与贡献
TBD


#### 许可证
TBD