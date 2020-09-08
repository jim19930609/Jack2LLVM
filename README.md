# Jack2LLVM

## Gen Executable
LLVM has no linker supported yet, meaning we have to support that in our implementation.
Suppose only one single main.ll file is generated:

1. llc main.ll -filetype=obj -o main.o
2. g++ -v main.o -o main.out

## TODO
1. Code polish
2. Test framework & test cases
3. [New Features]
- Cast: ptr and basic types
- Inheritance
- Polymorph
