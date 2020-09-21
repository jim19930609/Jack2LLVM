
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

  std::shared_ptr<llvm::Module> getModule() { return Module; }

  JackRealVisitor() {
    // Suppose we only handle one single module
    // TODO: Add linking and relocation [future]
    Module = std::make_unique<llvm::Module>("Yet Another Module", Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(Context);
  }

  llvm::Value* variableLookup(std::string name);
  llvm::Type*  getVarType(antlr4::tree::TerminalNode* var_type, JackParser::ClassNameContext* class_type_ctx);

private:
  // LLVM Members
  llvm::LLVMContext Context;
  std::shared_ptr<llvm::Module> Module;
  std::shared_ptr<llvm::IRBuilder<>> Builder;

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

    // Map class member name to index
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, size_t>> class_member_name_to_index;
    
    // Raw name to actual registered name 
    std::unordered_map<llvm::Type*, std::unordered_map<std::string, std::string>> class_var_func_name_mapping;

    std::string current_class_name;
    std::string current_function_name;
    std::string function_decorator;
  };
  
  LoweringHints visitorHelper;

};

