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


# 文档

## 调用



## 语法结构

```mermaid
graph TB
Root([root]) --> LProdCallList[LProdCallList] --> LProdCall1[LProdCall1] --> s1[SymName]
LProdCallList --> LProdCall2[LProdCall2]
LProdCallList --> LProdCall3[...]
LProdCall1 --> ArgList[ArgList] --> ArgExpr1[ArgExpr1]
ArgList --> ArgExpr2[ArgExpr2]
ArgList --> ArgExpr3[...]

Root --> LSystem[LSystem] --> LProduction1[LProduction1] --> SymSrc[SymSrc] --> s2[SymName]
LSystem --> LProduction2[LProduction2]
LSystem --> LProduction3[...]

SymSrc --> ParamList[ParamList] --> Param1[Param1]
ParamList --> Param2[Param2]
ParamList --> Param3[...]

LProduction1 --> SymDstList[SymDstList] --> SymDst1[SymDst1] --> s3[SymName]
SymDstList --> SymDst2[SymDst2]
SymDstList --> SymDst3[...]

SymDst1 --> ParamMapList[ParamMapList] --> ParamMap1[ParamMap1]
ParamMapList --> ParamMap2[ParamMap2]
ParamMapList --> ParamMap3[...]
```

### LProduction示例

`F(x,y) -> A(x+y,x-y,x*y)F(x/y,x+1)B((x+y+z)/3)`

```mermaid
graph TB
    T([LProduction]) --> A
    T --> B_dst_list
    A["F(x,y)"] --> A_symname[F]
    A --> A_p_list["(x,y)"] --> A_p1[x]
    A_p_list --> A_p2[y]
    B_dst_list["A(x+y,x-y,x*y)F(x/y,x+1)B((x+y+z)/3)"] --> B_dst1["A(x+y,x-y,x*y)"] --> S_name[A]
    B_dst_list --> B_dst2["F(x/y,x+1)"]
    B_dst_list --> B_dst3["B((x+y+z)/3)"]
    B_dst1 --> param_map_list["(x+y,x-y,x*y)"] --> param_map1["x+y"]
    param_map_list --> param_map2["x-y"]
    param_map_list --> param_map3["x*y"]
```
