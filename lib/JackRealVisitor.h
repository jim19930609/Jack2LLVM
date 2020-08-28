
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

  virtual antlrcpp::Any visitClassDec(JackParser::ClassDecContext *ctx) override;
  virtual antlrcpp::Any visitClassVarDec(JackParser::ClassVarDecContext *ctx) override;
  virtual antlrcpp::Any visitType(JackParser::TypeContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineDec(JackParser::SubroutineDecContext *ctx) override;
  virtual antlrcpp::Any visitParameterList(JackParser::ParameterListContext *ctx) override;
  virtual antlrcpp::Any visitSubroutineBody(JackParser::SubroutineBodyContext *ctx) override;


  virtual antlrcpp::Any visitStatements(JackParser::StatementsContext *ctx) override;

  virtual antlrcpp::Any visitStatement(JackParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLetStatement(JackParser::LetStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIfStatement(JackParser::IfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitWhileStatement(JackParser::WhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDoStatement(JackParser::DoStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReturnStatement(JackParser::ReturnStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpression(JackParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTerm(JackParser::TermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSubroutineCall(JackParser::SubroutineCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpressionList(JackParser::ExpressionListContext *ctx) override {
    return visitChildren(ctx);
  }
  
  // Identifiers, Names...
  virtual antlrcpp::Any visitSubroutineName(JackParser::SubroutineNameContext *ctx) override;
  virtual antlrcpp::Any visitClassName(JackParser::ClassNameContext *ctx) override;
  virtual antlrcpp::Any visitVarName(JackParser::VarNameContext *ctx) override;
  virtual antlrcpp::Any visitVarDec(JackParser::VarDecContext *ctx) override;

  JackRealVisitor() {
    // Suppose we only handle one single module
    // TODO: Add linking and relocation [future]
    Module = std::make_unique<llvm::Module>("Yet Another Module", Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(Context);
  }

private:
  // LLVM Members
  llvm::LLVMContext Context;

  std::shared_ptr<llvm::Module> Module;
  std::shared_ptr<llvm::IRBuilder<>> Builder;

  llvm::Function* currentFunction;

  class Symtab {
    public:
      void reset() {};
      void insert() {};
      void get() {};
    private:
      //Storage Type

  };

  // ------- //
  // Symtabs //
  // ------- //
  // global variables: symtab_g
  // -contain 'static' variables
  // -reset with Module creation
  Symtab symtab_g;
  
  // class variables : symtab_c
  // -contain 'field' variables
  // -reset with ClassDec
  Symtab symtab_c;
  
  // local variables : symtab_l
  // -contain 'local' and 'argument' variables
  // -reset with SubroutineDec
  Symtab symtab_l;
  
};

