#include <iostream>
#include <fstream>
#include <string>

#include "antlr4-runtime.h"
#include "JackLexer.h" 
#include "JackParser.h"
#include "JackRealVisitor.h"

using namespace antlr4;

static void throw_with_message(std::string message) {
  std::cout << "Exception: " << message << std::endl;
  throw;
}

int main(int argc, const char *args[])
{
    std::ifstream InFile(args[1]);
    if(!InFile.is_open()) throw_with_message("Unable to open file");

    antlr4::ANTLRInputStream input(InFile); 
    JackLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    JackParser parser(&tokens);
    
    // Parse Tree
    auto classCtx = parser.classDec();
    
    // Print the parse tree in Lisp format.
    JackRealVisitor visitor;
    visitor.visitClassDec(classCtx);
    
    return 0; 
}
