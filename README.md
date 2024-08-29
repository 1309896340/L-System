# L-System

D0L-System : 确定性、上下文无关

<br/>



# 实现一个简单的D0L-System

示例：雪花分形

`Varname: F`

`Axiom: F`

`Production: F -> F+F--F+F`

需要用lexy解析这个表达式

## 类设计

首先，对LSystem的初始化，需要注册所有会用到的变量名，例如这里的 `F` 就是变量名

为LSystem类添加一个 `string iterate(const string &p)` 的方法，这个方法用于对字符串进行迭代计算

增加`string tmp` 用来存储中间量，增加 `string next()` 方法用于调用 `iterate` 更新 `tmp` 并返回，增加 `void reset()` 方法用于重置 `tmp` 以及其他所有迭代过程中可能的状态量

但是这么做，显得 `iterate()` 太过独立，但它要做的事相当明确，就是由一个输入串，根据LRule进行替换，生成新串。将 `iterate()` 独立出来的好处，是可以通过将其指定为虚函数，以派生不同规则的LSystem子类

## 初步构思实现

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

### 解析L-System产生式的定义

`Sym` : 产生式的左半部分

`SymName` : `Sym` 的名称部分，以字母、下划线开头，后续字母、数字、下划线的组合

`ParameterList` : 由圆括号包裹任意数量个 `ParamItem` 构成

`ParamItem` : 符号的参数项，指定 `Sym` 的形式参数名，以字母、下划线开头，后续字母、数字、下划线的组合


<br/>

`SymMap` : 产生式的右半部分，同样以 `SymName` 开头并紧跟一个可选择的 `ParameterMappedList`，但是其item定义有所区别，映射到的目标是原 `ParamItem` 构成的表达式

`ParameterMappedList` : 左侧形式参数映射到的右侧的参数列表，其中的每一项为 `MathExpr`

`MathExpr` : 数学表达式，由 `ParamItem`、`Number`、 递归的 `MathExpr` 构成




## 构建语法树

表达式的作用域
```cpp
    struct environment{
        // 解析出来的L-System 生成式

    };
```

数学表达式语法树的基类

```cpp
    struct expr{
        virtual ~expr()=default;
        virtual float evaluate() const = 0;
    };
```

表达式中对L-System产生式的描述方法：将其表示为长度n的语法树数组，即 `vector<expr>` ，n为形参数量。



## lexy分支解析

`dsl::if_` `dsl::else_`  `dsl::try_`  `dsl::opt`
`dsl::peek`  `operator|` `operator>>`

`branch-rule >> branch`
