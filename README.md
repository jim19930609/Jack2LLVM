# Jack2LLVM

# 简介
本项目实现面向对象语言Jack的编译器前端，将其翻译为LLVMIR，并通过llc工具接入X86后端，经过ld链接后产生可执行代码。

## 1. 语言
Jack是一门面向对象语言，其语言设计主体来自Nand2Tetris（https://www.nand2tetris.org/） ，但原版Jack并不包含面向对象的支持，同时其他语法特性也比较简单。我个人在此基础上进行了修改，在面向对象方面添加了对继承、多态（虚函数）的支持；而在基础语法方面多支持了boolean、char等类型，以及castExpression进行类型转换。

详细的语法请参照Jack.g4，基本是正则表达式的语法。也可以参考下方的简略语法对照图作为入口：

![image](https://user-images.githubusercontent.com/22334008/152664632-80987b82-80ba-4bc4-8aba-5d87a10d450e.png)

## 2. 系统调用支持
得益于LLVMIR良好的抽象，在ABI Compatible的基础上，我们可以复用C Library中的所有函数，包括C语言中封装好的系统函数。

目前我们支持system.putchar(), system.puts(), system.getchar()，system.rand()。在Jack LLVMIR中我们只是按照对应的函数签名对这些系统函数进行了声明，在后期通过gcc的链接器，与C语言的函数库进行链接，从而完成对C语言相应系统函数的调用。

## 3. 编译流程图

![image](https://user-images.githubusercontent.com/22334008/152664979-2e8df4d9-d4cb-4d6b-9566-dcffeac57625.png)

1. game.jack中是用Jack语法写作的应用程序。
2. 通过本项目实现的编译器前端jack2llvm，将game.jack转化为game.ll，此时已经是Jack LLVMIR。
3. 通过LLVM工具llc将game.ll编译为game.o。llc是将LLVMIR接入对应Architecture（这里是X86）后端并按照对应文件类型（这里是obj）进行代码生成的工具。
4. obj文件不能直接运行，因此我们需要写一些driver code。这里采用最简单的方式，写一个test.cpp，在里面调用game.jack的入口(Main_main()函数)。
5. 通过gcc将game.o与test.cpp链接成可执行文件。

## 4. Parsing流程
jack2llvm编译器前端的一些实现思路，有兴趣调试编译器的朋友可供参考：

首先是不含继承的Parsing流程：
![image](https://user-images.githubusercontent.com/22334008/152665099-0b98a07c-16ab-4409-b791-b34cb0c8cf75.png)

对于继承来说，主要是处理虚函数和成员。含有继承的Parsing流程：
![image](https://user-images.githubusercontent.com/22334008/152665111-bc36a64d-5be4-4ea3-92a7-152eeb92b0de.png)

## 5. 如何上手
你可以通过Demo: “tests/Demo/MineSweeping”作为例子来调试Jack语言或编译器。这是一个非常简单的10x10扫雷游戏：

<img src="https://user-images.githubusercontent.com/22334008/152664844-916f8a15-0e6f-4d2e-b0d0-0b18ab13b8ff.gif" width="400">

除此以外，在"tests"文件夹下还有针对所有语言特性的单测。其中“tests/InheritanceUnitTests/ComplexCallMethodTest”包含了继承、多态等相对复杂一些的特性，同时也可以直接编译运行，可以作为参考。

其余部分的单测只是检查Jack LLVMIR的正确性，如果想要在X86机器上运行这些单测，需要额外写一个test.cpp作为driver。详情可以参考“tests/InheritanceUnitTests/ComplexCallMethodTest/test.cpp”以及“tests/InheritanceUnitTests/ComplexCallMethodTest/Makefile”进行编译配置。

# 编译安装
目前只在Linux（Ubuntu）环境下进行过测试。
1. 下载本项目
```
git clone https://github.com/jim19930609/Jack2LLVM.git
```

2. Install dependencies

[MacOS]
```
brew install glog
brew install java
brew install wget
```

[Linux]
```
sudo apt update
sudo apt install aptitude
sudo apt install clang

sudo aptitude update
sudo aptitude install -y libgflags-dev
sudo aptitude install -y libgoogle-glog-dev
sudo aptitude install -y default-jre
```

Set default compiler to clang
```
export CXX=clang++
```

3. Download pre-compiled llvm library
MacOS:
```
wget https://github.com/jim19930609/Jack2LLVM/releases/download/llvm/llvm-10.0.0-m1.zip
tar -xf llvm-10.0.0-m1.zip && mv llvm-10.0.0-m1 Jack2LLVM/third_party
```

Linux:
```
wget https://github.com/jim19930609/Jack2LLVM/releases/download/llvm/taichi-llvm-10.0.0-linux.zip
unzip taichi-llvm-10.0.0-linux.zip && mv taichi-llvm-10.0.0-linux Jack2LLVM/third_party
```

4. 编译Jack2LLVM
```
mkdir build 
cd build && cmake .. && make -j
```

5. 编译并运行Demo
```
cd tests/Demo/MineSweeping/
make
./main
```
