
// Generated from Jack.g4 by ANTLR 4.8


#include "JackRealVisitor.h"
#include "llvm/IR/Type.h"

antlrcpp::Any JackRealVisitor::visitClassDec(JackParser::ClassDecContext *ctx) {
  // Class Name
  JackParser::ClassNameContext* class_name_ctx = ctx->className();
  std::string class_name_text = this->visitClassName(class_name_ctx);

  // ClassDec Members
  const std::vector<JackParser::ClassVarDecContext *>& class_var_decs = ctx->classVarDec();
  const std::vector<JackParser::SubroutineDecContext *>& subroutine_decs = ctx->subroutineDec();
  
  // ----------------- //
  // Parse ClassVarDec //
  // ----------------- //
  // This will return a vector<Type> containing all the declared variables
  // ClassDec then register a struct definition in LLVM::Module
  // Collect static/field vars seperately from ClassVarDec
  std::vector<std::pair<std::string, llvm::Type*>> static_class_vars;
  std::vector<std::pair<std::string, llvm::Type*>> field_class_vars;
  for(JackParser::ClassVarDecContext* class_var_dec_ctx : class_var_decs) {
    
    std::vector<std::pair<std::string, llvm::Type*>> class_vars = this->visitClassVarDec(class_var_dec_ctx);
    assert(class_vars.size() > 0 && "Empty classVarDec is not allowed");

    std::string decorator = class_vars[0].first;
    for(size_t class_var_index=1; class_var_index<class_vars.size(); class_var_index++) {
      if(decorator == "static") {
        static_class_vars.emplace_back(class_vars[class_var_index]);

      } else if (decorator == "field") {
        field_class_vars.emplace_back(class_vars[class_var_index]);
      
      } else {
        assert(false && "Decorator types can only be either 'static' or 'field'");

      }
    }
  }
  
  // For field vars
  // register a struct type in llvm::Module
  std::vector<llvm::Type*> struct_members;
  for(const auto& kv : field_class_vars) {
    struct_members.push_back(kv.second);
  }
  llvm::StructType* registered_class_type = llvm::StructType::create(this->Context, struct_members, class_name_text, true);
  assert(registered_class_type && "Unable to create class StructType during ClassDec");

  // As for static vars
  // register a global var with class_name prefix in LLVM::Module
  for(const auto& kv : static_class_vars) {
    llvm::Type* global_type = kv.second;
    std::string global_name = kv.first;
    // Apply special prefix to static member var
    // Then make it a global var
    global_name = class_name_text + "." + global_name;
    llvm::Constant* declared_global_var = this->Module->getOrInsertGlobal(global_name, global_type);
    assert(declared_global_var && "Unable to register static member as global variable");
  }

  // ------------------- //
  // Parse SubroutineDec //
  // ------------------- //

  /*
  std::vector<Type *> Contents = {Type::getDoubleTy(Context), Type::getVoidTy(Context)};
  StructType* x = StructType::get(Context, Contents, true);
  
  std::vector<Type *> C = {x, x};
  StructType* y = StructType::get(Context, C, true);
  */
  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitClassVarDec(JackParser::ClassVarDecContext *ctx) {
  // ------------------------------- //
  // Resolve Decorator: statis/field //
  // ------------------------------- //
  antlr4::tree::TerminalNode* decorator = ctx->CLASSDECORATOR();
  antlr4::Token* tok = decorator->getSymbol();
  std::string tok_text = tok->getText();

  // --------------------- //
  // Resolve variable type //
  // --------------------- //
  // Find out the type
  // then create LLVM::Type correspondingly
  JackParser::TypeContext* type_ctx = ctx->type();
  llvm::Type* varType = this->visitType(type_ctx);

  // ----------------------- //
  // Construct return Vector //
  // ----------------------- //
  // vector = { {'static'/'field', nullptr}, {varname, type}, ... }
  std::vector<std::pair<std::string, llvm::Type*>> class_vars = { {tok_text, nullptr} };

  // Find out class variable name
  std::vector<JackParser::VarNameContext*> var_name_ctxs = ctx->varName();
  for(const auto var_name_ctx : var_name_ctxs) {
    std::string var_name_str = this->visitVarName(var_name_ctx);
    // Append {name, type} pairs to return vector
    class_vars.emplace_back(std::make_pair(var_name_str, varType));
  }

  return class_vars;
}

antlrcpp::Any JackRealVisitor::visitType(JackParser::TypeContext *ctx) {
  // --------------------------- //
  // Parse and return llvm::Type //
  // --------------------------- //
  antlr4::tree::TerminalNode* var_type = ctx->VARTYPES();
  JackParser::ClassNameContext* class_type_ctx = ctx->className();

  llvm::Type* varType;
  if(var_type) {
    // Create corresponding LLVM::Type
    // -Boolean is Int1
    // -Int     is Int32
    // -Char    is Int8
    antlr4::Token* var_type_tok = var_type -> getSymbol();
    std::string var_type_str = var_type_tok->getText();
    if(var_type_str == "int") {
      varType = llvm::Type::getInt32Ty(this->Context);
    } else if(var_type_str == "char") {
      varType = llvm::Type::getInt8Ty(this->Context);
    } else if(var_type_str == "boolean") {
      varType = llvm::Type::getInt1Ty(this->Context);
    } else {
      assert(false && "Class variable basic types can only be 'int', 'char' or 'boolean'");
    }

  } else if(class_type_ctx) {
    // Get registered llvm::structType from llvm::Module
    std::string identifier_text = this->visitClassName(class_type_ctx);
    
    varType = this->Module->getTypeByName(identifier_text);
    assert(varType && "Class identifier used before declared");

  } else {
    assert(false && "Class variable type has to be either 'var_type' or 'class_type'");
  }
  
  return varType;
}

antlrcpp::Any JackRealVisitor::visitClassName(JackParser::ClassNameContext *ctx) {
  // Parse and return variable name in std::string
  antlr4::tree::TerminalNode* class_name = ctx->ID();
  antlr4::Token* class_name_tok = class_name->getSymbol();
  std::string class_name_text = class_name_tok->getText();
  return class_name_text;
}

antlrcpp::Any JackRealVisitor::visitVarName(JackParser::VarNameContext *ctx) {
  // Parse and return variable name in std::string
  antlr4::tree::TerminalNode* var_name = ctx->ID();
  antlr4::Token* var_name_tok = var_name -> getSymbol();
  std::string var_name_str = var_name_tok->getText();
  return var_name_str;
}
