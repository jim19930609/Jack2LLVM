# Jack2LLVM

# 简介
本项目实现面向对象语言Jack的编译器前端，将其翻译为LLVMIR，并通过llc工具接入X86后端，经过ld链接后产生可执行代码。

## 1. 语言
Jack是一门面向对象语言，其语言设计主体来自Nand2Tetris（https://www.nand2tetris.org/） ，但原版Jack并不包含面向对象的支持，同时其他语法特性也比较简单。我个人在此基础上进行了修改，在面向对象方面添加了对继承、多态（虚函数）的支持；而在基础语法方面多支持了boolean、char等类型，以及castExpression进行类型转换。

详细的语法请参照Jack.g4，基本是正则表达式的语法。也可以参考下方的简略语法对照图作为入口：

![image](https://user-images.githubusercontent.com/22334008/152664632-80987b82-80ba-4bc4-8aba-5d87a10d450e.png)

## 2. 系统调用支持
得益于LLVMIR良好的抽象，在ABI Compatible的基础上，我们其实可以复用C Library中的所有函数，包括C语言中封装好的系统调用函数。

目前我们支持system.putchar(), system.puts(), system.getchar()，system.rand()。在Jack LLVMIR中我们只是按照对应的函数签名对这些系统函数进行了声明，在后期通过gcc的链接器，与C语言的函数库进行链接，从而完成对C语言相应系统函数的调用。

## 3. 编译流程图


## 4. 如何上手
你可以通过Demo: “tests/Demo/MineSweeping”作为例子来调试Jack语言、编译器。这是一个非常简单的10x10扫雷游戏：



## 后续开发计划
