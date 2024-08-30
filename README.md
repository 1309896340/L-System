# L-System简介

![](https://img.shields.io/badge/IDE-Visual%20Studio%20Code-blue) ![](https://img.shields.io/badge/Language-C++-yellow) ![](https://img.shields.io/github/license/1309896340/L-System.svg)

Lindenmayer System，简称L-System，一种字符串并行重写系统，由匈牙利植物学家Aristid Lindenmayer在其1968研究关于生长发育过程中细胞交互作用的数学模型[[1]](#参考链接)中使用的形式文法，后发展出一套形式体系，广泛用于各类生物的形态模拟，以及自相似形体的生成。

Lindenmayer的继任者Prusinkiewicz在1992年著有 _The Algorithm Beauty of Plants_ [[2]](#参考链接)一书，展示了许多精美的植物生成结构。

本项目基于C++17标准的lexy库[[3]](#参考链接)，实现带参数、确定性、上下文无关L-System解析生成器。

![LSystemExample](/assets/images/LSystemExample.png)

<br/>

### 参考链接
[1]. [Mathematical models for cellular interactions in development I. Filaments with one-sided inputs](https://doi.org/10.1016/0022-5193(68)90079-9)

[2]. [The Algorithm Beauty of Plants](http://algorithmicbotany.org/papers/abop/abop.pdf)

[3]. [lexy: C++ parsing DSL](https://lexy.foonathan.net/)


<br/>

# 日志

## 前期类设计

首先，对LSystem的初始化，需要注册所有会用到的变量名，例如这里的 `F` 就是变量名

为LSystem类添加一个 `string iterate(const string &p)` 的方法，这个方法用于对字符串进行迭代计算

增加`string tmp` 用来存储中间量，增加 `string next()` 方法用于调用 `iterate` 更新 `tmp` 并返回，增加 `void reset()` 方法用于重置 `tmp` 以及其他所有迭代过程中可能的状态量

但是这么做，显得 `iterate()` 太过独立，但它要做的事相当明确，就是由一个输入串，根据LRule进行替换，生成新串。将 `iterate()` 独立出来的好处，是可以通过将其指定为虚函数，以派生不同规则的LSystem子类

## 前期头脑风暴

D0LSystem的 `iterate()` 实现

将Production拆解为两个部分，与map的命名保持一致，左半部分称为 `key`，右半部分称为 `value`

`key`的语法，先举个例子，它可以是 `F(0, 1, 2, 3, 4.3)AC(3, 1.1, 2)`

其中 `F` 有5个参数，参数数量这是确定的，在最开始的 `Varname` 中就已经定义好的。后面紧跟的 `A` 可能存在歧义，它可能是一个无参数的独立变量，也可能是一个带3参数的名为 `AC` 的变量，区分它们的方法也是看 `Varname` 中的定义

如此分析，每个在 `Varname` 出现的变量都应该表示成一个函数，但是解析的函数应该如何在代码中实现？首先，它由一个函数名，和若干参数构成，两者是一体的，不能分开写，因此上文中的 LRule 的定义就不合适，应当用一个字符串整体来定义，就像matlab的 `function res = xxx()` 这样，我可以将它的语法规定为比如 `xxx(x, y, ...) -> yyy(y, x, ...)zzz(0.1x, 0.2y)`

注意到这个语法的特点，`key` 部分只有一个带参符号，但是 `value` 部分可以由多个带参符号构成，这类文法应该有个名字，可能叫 `LL(1)` 型文法，需要搜索了解一下

注意到一个问题，varname和production中对于符号的唯一性管理，对于一个带参数的符号，它应当是在production中进行具体的参数映射的定义，但是它同时也应该在varname中进行声明，以保证它和不带参数的符号不会发生冲突

重新考虑了一下，将varname直接删去，LSystem类直接通过解析LRule中的 `key` 来注册所有非终结符号1。但这么做的话，需要另外设计一个用于最终渲染的解析器。


先整理一下 `key` 部分的语法格式。首先，它只包含一个符号，这个符号可以带参数，也可以不带参数

注册符号表似乎不需要传递额外的参数，只需要解析 `LRule` 的 `key` 即可



## 语法规则整理

### 解析L-System产生式的语法设计和命名

参考例子： `F(x,y) -> A(0.1*x,0.2*y,x-y)F(x-y,x+y)B((x+y)/2)`

`Sym` : 产生式的左半部分，以 `SymName` 开头并紧跟一个可为空的 `(` `ParamList` `)`

`SymName` : `Sym` 的名称部分，以字母、下划线开头，后续字母、数字、下划线的组合

`ParamList` : 由圆括号包裹任意数量个 `ParamItem` 构成

`ParamItem` : 符号的参数项，指定 `Sym` 的形式参数名，以字母、下划线开头，后续字母、数字、下划线的组合

`SymMap` : 产生式的右半部分，同样以 `SymName` 开头但紧跟一个可为空的 `(` `ParamMappedList` `)`

`ParamMappedList` : 左侧形式参数映射到的右侧的参数列表，以 `,` 为分隔符，每项为 `MathExpr`

`MathExpr` : 数学表达式，其最小构成单元为 `ParamItem`、`Number`、 带括号的嵌套 `MathExpr` 

`SymMapList` : 多个 `SymMap` 的无分隔符拼接

`LProduction` : 完整的L-System产生式，由 `->` 左侧的 `Sym` 和右侧的 `SymMapList` 构成

### Cpp-Lexy的实现思路





## 构建语法树

表达式的作用域
```cpp
    struct environment{
        // 一个LProduction中，每个Sym的每个ParamItem都对应一个SymMap
        // 待构建 LProduction 对应的执行类
    };
```

数学表达式语法树的基类

```cpp
    struct expr{
        virtual ~expr()=default;
        virtual float evaluate() const = 0;
    };
```

在lexy中，表达式产生式继承自 `lexy::expression_production` ，在这个结构下，将各类运算分别用以下基类派生
1. `dsl::infit_op_left` : 从左向右结合的运算，例如 (a ? b) ? c
2. `dsl::infit_op_right` : 从右向左结合的运算，例如 a ? (b ? c)
3. `dsl::infix_op_single` : 双目运算，与1,2相区别，遇到 a ? b ? c 的情况会报错
4. `dsl::infix_op_list` : 需要手动指定结合方式的运算
5. `dsl::prefix_op` : 前缀单目运算，例如 ? a
6. `dsl::postfix_op` : 后缀单目运算，例如 a ?

表达式产生式需要通过 `operation` 成员来指定最低优先级运算，并和其他自定义产生式一样有 `value` 成员来构造解析结果



