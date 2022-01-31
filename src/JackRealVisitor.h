
// Generated from Jack.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "JackVisitor.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

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
  virtual antlrcpp::Any visitCastStatement(JackParser::CastStatementContext *ctx) override;

  // Expression Level
  virtual antlrcpp::Any visitExpression(JackParser::ExpressionContext *ctx) override;
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
  }

  // Member Access
  llvm::LLVMContext& getContext() { return context_; }
  llvm::Module& getModule() { return module_; }
  llvm::IRBuilder<>& getBuilder() { return builder_; }

  llvm::Value* variableLookup(std::string name);
  llvm::Type*  getVarType(antlr4::tree::TerminalNode* var_type, JackParser::ClassNameContext* class_type_ctx);

private:
  // LLVM Members
  llvm::LLVMContext context_;
  llvm::Module module_;
  llvm::IRBuilder<> builder_;

  struct LoweringHints {

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
    
    // local variables : symtab_a
    // -contain 'argument' variables
    // -reset with SubroutineDec
    std::unordered_map<std::string, llvm::Value*> symtab_a;

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
    
    // [Global] demangled name
    std::unordered_map<llvm::Type*, std::vector<std::string>> class_vtable_function_order;

    std::string current_class_name;
    std::string current_function_name;
    std::string function_decorator;
  };
  
  LoweringHints visitorHelper;

};

