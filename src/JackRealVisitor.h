
// Generated from Jack.g4 by ANTLR 4.8

#pragma once

#include <glog/logging.h>
#include "llvm/Support/raw_ostream.h"
#include "antlr4-runtime.h"
#include "JackVisitor.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

inline void print_llvm_type(llvm::Type* type) {
  std::string print_str;
  llvm::raw_string_ostream rso(print_str);
  type->print(rso);
  VLOG(6) << rso.str();
}

enum SYM_TYPE {
    LOCAL = 0,
    CLASS_MEMBER = 1,
    GLOBAL = 2,
};

/**
 * This class provides an empty implementation of JackVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 * 
 * Note that the translation unit of JackRealVisitor is at Class-level, and there is another 
 * upper-level controller that handles cross-class compilations
 */
class  JackRealVisitor : public JackVisitor {
public:
  // Top Level or Class Level
  virtual antlrcpp::Any visitClassDec(JackParser::ClassDecContext *ctx) override;
  virtual antlrcpp::Any visitClassVarDec(JackParser::ClassVarDecContext *ctx) override;
  virtual antlrcpp::Any visitType(JackParser::TypeContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineDec(JackParser::SubroutineDecContext *ctx) override;
  virtual antlrcpp::Any visitParameterList(JackParser::ParameterListContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineBody(JackParser::SubroutineBodyContext *ctx) override;

  // Statement Level
  virtual antlrcpp::Any visitStatements(JackParser::StatementsContext *ctx) override;
  virtual antlrcpp::Any visitStatement(JackParser::StatementContext *ctx) override;
  virtual antlrcpp::Any visitLetStatement(JackParser::LetStatementContext *ctx) override;
  virtual antlrcpp::Any visitIfStatement(JackParser::IfStatementContext *ctx) override;
  virtual antlrcpp::Any visitWhileStatement(JackParser::WhileStatementContext *ctx) override;
  virtual antlrcpp::Any visitDoStatement(JackParser::DoStatementContext *ctx) override;
  virtual antlrcpp::Any visitReturnStatement(JackParser::ReturnStatementContext *ctx) override;
  virtual antlrcpp::Any visitPutsStatement(JackParser::PutsStatementContext *ctx) override;

  // Expression Level
  virtual antlrcpp::Any visitExpression(JackParser::ExpressionContext *ctx) override;
  virtual antlrcpp::Any visitCastExpression(JackParser::CastExpressionContext *ctx) override;
  virtual antlrcpp::Any visitTerm(JackParser::TermContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineCall(JackParser::SubroutineCallContext *ctx) override;
  virtual antlrcpp::Any visitExpressionList(JackParser::ExpressionListContext *ctx) override;
  
  // Identifiers, Names...
  virtual antlrcpp::Any visitArrayType(JackParser::ArrayTypeContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineName(JackParser::SubroutineNameContext *ctx) override;
  virtual antlrcpp::Any visitClassName(JackParser::ClassNameContext *ctx) override;
  virtual antlrcpp::Any visitVarName(JackParser::VarNameContext *ctx) override;
  virtual antlrcpp::Any visitVarDec(JackParser::VarDecContext *ctx) override;
  
  // Constructor
  JackRealVisitor() : module_("Yet Another Module", context_), builder_(context_){
      declareSystemFunctions();
  }

  // Member Access
  llvm::LLVMContext& getContext() { return context_; }
  llvm::Module& getModule() { return module_; }
  llvm::IRBuilder<>& getBuilder() { return builder_; }

  llvm::Value* variableLookup(std::string name, SYM_TYPE* type = nullptr);
  llvm::Type*  getVarType(antlr4::tree::TerminalNode* var_type, JackParser::ClassNameContext* class_type_ctx);

  void declareSystemFunctions() {
    getModule().getOrInsertFunction("puts", llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(getContext()), llvm::PointerType::get(llvm::Type::getInt8Ty(getContext()), 0), false));
  }

private:
  // LLVM Members
  llvm::LLVMContext context_;
  llvm::Module module_;
  llvm::IRBuilder<> builder_;

  struct LoweringHints {
    
    llvm::BasicBlock* GetBlockBefore() {
        if(block_before_stack_.size() > 0) {
            return block_before_stack_.back();
        }
        return nullptr;
    }

    void PopBlockBefore(){
        block_before_stack_.pop_back();
    }

    void SetBlockBefore(llvm::BasicBlock* BB) {
        block_before_stack_.push_back(BB);
    }

    // ------- //
    // Symtabs //
    // ------- //
    // global variables: symtab_g
    // -contain 'static' variables
    // -reset with Module creation
    // Access directly from Module

    // class variables : symtab_c
    // -contain 'field' variables
    // These var will be accessed via GEP to 'this'
    // -reset with ClassDec
    std::unordered_map<std::string, size_t> symtab_c;
    
    // local variables : symtab_l
    // -contain 'local' variables
    // -reset with SubroutineDec
    std::unordered_map<std::string, llvm::AllocaInst*> symtab_l;

    // BlockBefore Stack
    std::vector<llvm::BasicBlock*> block_before_stack_;
    
    // [Global] Map class member name to index
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, size_t>> class_member_name_to_index;
    
    // [Global] Raw Name to Mangled Name
    // class methods name mangling
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, std::string>> class_func_name_mapping;
    // class functions name mangling
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, std::string>> static_func_name_mapping;
    
    // [Global] static class member name mangling
    // example: Car.wheel -> Car_wheel
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, std::string>> class_globalvar_name_mapping;
    
    // [Global] vtable function name mapping
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, size_t>> class_vtable_name_mapping;

    // [Global] vtable address
    std::unordered_map<llvm::Type*, llvm::Value*> class_vtable_address;

    std::string current_class_name;
    std::string current_function_name;
    std::string function_decorator;
  };
  
  LoweringHints visitorHelper;

};

