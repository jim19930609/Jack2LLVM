ANTLR=/usr/bin/java -jar antlr-4.7-complete.jar
OUTPUT=output
GENERATED=lib
GRAMMAR=Jack
MAIN=main

# here is where you plug in the runtime for your OS
RUNTIME=runtime-osx
CC=g++
CCARGS=-c -I $(RUNTIME)/antlr4-runtime/ -I $(GENERATED) -std=c++14	
LDARGS=-g
LIBS=$(RUNTIME)/lib/libantlr4-runtime.a
LLVMINCLUDE=`llvm-config --cflags` -std=c++14
LLVMLINK=`llvm-config --ldflags --system-libs --libs core`

runtime: libdirs $(MAIN).cpp
	# basic lexer & parser
	$(CC) $(CCARGS) $(GENERATED)/$(GRAMMAR)Lexer.cpp -o $(OUTPUT)/$(GRAMMAR)Lexer.o 
	$(CC) $(CCARGS) $(GENERATED)/$(GRAMMAR)Parser.cpp -o $(OUTPUT)/$(GRAMMAR)Parser.o 
	$(CC) $(CCARGS) $(GENERATED)/$(GRAMMAR)Visitor.cpp -o $(OUTPUT)/$(GRAMMAR)Visitor.o

	# visitor lib
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(GENERATED)/$(GRAMMAR)ExpressionVisitor.cpp -o $(OUTPUT)/$(GRAMMAR)ExpressionVisitor.o
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(GENERATED)/$(GRAMMAR)StatementVisitor.cpp -o $(OUTPUT)/$(GRAMMAR)StatementVisitor.o
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(GENERATED)/$(GRAMMAR)ClassVisitor.cpp -o $(OUTPUT)/$(GRAMMAR)ClassVisitor.o
	
	# main lib
	$(CC) $(CCARGS) $(LLVMINCLUDE) $(MAIN).cpp  -o $(OUTPUT)/$(MAIN).o 
	$(CC) $(LDARGS) $(LLVMLINK) $(OUTPUT)/$(MAIN).o $(OUTPUT)/$(GRAMMAR)ClassVisitor.o $(OUTPUT)/$(GRAMMAR)StatementVisitor.o $(OUTPUT)/$(GRAMMAR)ExpressionVisitor.o $(OUTPUT)/$(GRAMMAR)Lexer.o $(OUTPUT)/$(GRAMMAR)Visitor.o $(OUTPUT)/$(GRAMMAR)Parser.o $(LIBS) -o $(MAIN).out

gen: dirs $(GRAMMAR).g4
	$(ANTLR) -Dlanguage=Cpp -o $(GENERATED) $(GRAMMAR).g4 -no-listener -visitor

libdirs:
	mkdir -p $(OUTPUT) 

dirs:
	mkdir -p $(GENERATED) 

clean:
	rm -rf $(MAIN).out
	rm -rf $(OUTPUT)

s-clean: clean
	rm -rf $(GENERATED)
