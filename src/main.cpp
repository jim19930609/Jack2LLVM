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
    int num_files = argc - 1;
    
    // Print the parse tree in Lisp format.
    JackRealVisitor visitor;
    for(int i = 0; i < num_files; i++) {
        VLOG(6) << "Parsing input file: " << args[1];
        std::ifstream InFile(args[i+1]);
        if(!InFile.is_open()) throw_with_message("Unable to open file");

        antlr4::ANTLRInputStream input(InFile); 
        JackLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        JackParser parser(&tokens);

        // Parse Tree
        auto classCtx = parser.classDec();
        
        VLOG(6) << "Parsed Jack Grammar to AST";

        visitor.visitClassDec(classCtx);

        VLOG(6) << "Finished Parsing input file: " << args[1];
    }
    
    /* ---- Dumps ---- */ 
    auto& module = visitor.getModule();
    module.print(llvm::outs(), nullptr);

    return 0; 
}
