#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <unordered_set>

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

static std::unordered_set<std::string> preprocessed_files;

void preprocess_and_compile(JackRealVisitor& visitor, std::string& filepath) {  
    auto pos = filepath.rfind("/");
    std::string base_dir = filepath.substr(0, size_t(pos + 1));
    
    std::ifstream InFile(filepath);
    
    if(!InFile.is_open()) throw_with_message("Unable to open file");
    
    std::stringstream buffer;
    buffer << InFile.rdbuf();

    std::string file_content = buffer.str();
        
    /* ------------- */
    // Preprocessing //
    /* ------------- */
    // 1. Recursively parse "#include ..." and compile wrt topological order
    std::string deli_s = "#include";
    std::string deli_e = "\n";

    while(1) {
        auto deli_pos_s = file_content.find(deli_s);
        auto deli_pos_e = file_content.find(deli_e);
        
        if(deli_pos_s == file_content.npos || deli_pos_e == file_content.npos) {
            break;
        }
        
        size_t size_of_include = 9;
        std::string include_filename = file_content.substr(deli_pos_s + size_of_include, deli_pos_e - deli_pos_s - size_of_include);
        std::string include_filepath = base_dir + include_filename;

        if(preprocessed_files.count(include_filepath)) {
            continue;
        } 
        preprocessed_files.insert(include_filepath);
        file_content = file_content.substr(deli_pos_e, file_content.size() - deli_pos_e);
        
        // Recursively call preprocess_and_compile()
        preprocess_and_compile(visitor, include_filepath);
    }

    // Compile rest of the file content
    antlr4::ANTLRInputStream input(file_content); 
    JackLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    JackParser parser(&tokens);

    // Parse Tree
    auto classCtx = parser.classDec();
    
    VLOG(6) << "Parsed Jack Grammar to AST";

    visitor.visitClassDec(classCtx);
}

int main(int argc, const char *args[])
{
    assert(argc == 2);
    VLOG(6) << "Parsing input file: " << args[1];
    
    // Print the parse tree in Lisp format.
    JackRealVisitor visitor;
    std::string main_filename = std::string(args[1]);

    preprocess_and_compile(visitor, main_filename);

    /* ------------- */
    // Parsing Start //
    /* ------------- */
    
    /* ---- Dumps ---- */ 
    auto& module = visitor.getModule();
    module.print(llvm::outs(), nullptr);
    
    return 0; 
}
