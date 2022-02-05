JAVA=/usr/bin/java
BUILD=build
SRC=src
GRAMMAR=Jack
MAIN=main

LANGSRC=$(SRC)/language_src
ANTLRLIB=$(SRC)/runtime_lib/antlr-4.9.3-complete.jar
RUNTIMELIB=$(SRC)/runtime_lib/runtime-linux

# here is where you plug in the runtime for your OS
ANTLR=$(JAVA) -jar $(ANTLRLIB)
CC=/usr/local/gcc-8.2/bin/g++
CXX=/usr/local/gcc-8.2/bin/g++
CCARGS=-c -I $(RUNTIMELIB)/antlr4-runtime/ -I $(LANGSRC) -I $(SRC) -std=c++14
LDARGS=-g
SRCS=$(RUNTIMELIB)/lib/libantlr4-runtime.a -lglog
LLVMINCLUDE=`llvm-config --cflags` -std=c++14
LLVMLINK=`llvm-config --ldflags --system-libs --libs core`


all: language runtime

runtime: build-dirs $(MAIN).cpp
	# basic lexer & parser
	$(CC) $(CCARGS) $(LANGSRC)/$(GRAMMAR)Lexer.cpp -o $(BUILD)/$(GRAMMAR)Lexer.o 
	$(CC) $(CCARGS) $(LANGSRC)/$(GRAMMAR)Parser.cpp -o $(BUILD)/$(GRAMMAR)Parser.o 
	$(CC) $(CCARGS) $(LANGSRC)/$(GRAMMAR)Visitor.cpp -o $(BUILD)/$(GRAMMAR)Visitor.o

	# visitor lib
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(SRC)/$(GRAMMAR)ExpressionVisitor.cpp -o $(BUILD)/$(GRAMMAR)ExpressionVisitor.o
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(SRC)/$(GRAMMAR)StatementVisitor.cpp -o $(BUILD)/$(GRAMMAR)StatementVisitor.o
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(SRC)/$(GRAMMAR)ClassVisitor.cpp -o $(BUILD)/$(GRAMMAR)ClassVisitor.o
	
	# main lib
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(MAIN).cpp  -o $(BUILD)/$(MAIN).o 
	$(CC) $(LDARGS) $(LLVMLINK) $(BUILD)/$(MAIN).o $(BUILD)/$(GRAMMAR)ClassVisitor.o $(BUILD)/$(GRAMMAR)StatementVisitor.o $(BUILD)/$(GRAMMAR)ExpressionVisitor.o $(BUILD)/$(GRAMMAR)Lexer.o  $(BUILD)/$(GRAMMAR)Visitor.o $(BUILD)/$(GRAMMAR)Parser.o $(SRCS) -o $(BUILD)/jack2llvm

language: lang-dirs $(GRAMMAR).g4
	$(ANTLR) -Dlanguage=Cpp -o $(LANGSRC) $(GRAMMAR).g4 -no-listener -visitor

build-dirs:
	mkdir -p $(BUILD)

lang-dirs:
	mkdir -p $(LANGSRC)

clean:
	rm -rf $(BUILD)

s-clean: clean
	rm -rf $(LANGSRC)
