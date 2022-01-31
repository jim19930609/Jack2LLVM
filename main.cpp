#include <iostream>
#include <fstream>
#include <string>

#include "llvm/Support/raw_ostream.h"
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
    VLOG(6) << "Opened input file: " << args[1];

    antlr4::ANTLRInputStream input(InFile); 
    JackLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    JackParser parser(&tokens);

    // Parse Tree
    auto classCtx = parser.classDec();
    
    VLOG(6) << "Parsed Jack Grammar to AST";
    
    // Print the parse tree in Lisp format.
    JackRealVisitor visitor;

    VLOG(6) << "Created JackRealVisitor";

    visitor.visitClassDec(classCtx);

    VLOG(6) << "Parsed Jack into LLVMIR";
  
    /* ---- Dumps ---- */ 
    auto& module = visitor.getModule();
    module.print(llvm::outs(), nullptr);

    return 0; 
}
