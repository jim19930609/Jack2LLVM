#include "JackRealVisitor.h"
#include "llvm/IR/Type.h"

using namespace llvm;

antlrcpp::Any JackRealVisitor::visitStatements(JackParser::StatementsContext *ctx) {
  std::vector<JackParser::StatementContext*> statement_ctxs = ctx->statement();
  for(const auto& statement_ctx : statement_ctxs) {
    this->visitStatement(statement_ctx);
  }

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitStatement(JackParser::StatementContext *ctx) {

  JackParser::LetStatementContext* let_ctx = ctx->letStatement();
  if(let_ctx) {
    this->visitLetStatement(let_ctx);
  }

  JackParser::IfStatementContext* if_ctx = ctx->ifStatement();
  if(if_ctx) {
    this->visitIfStatement(if_ctx);
  }

  JackParser::WhileStatementContext* while_ctx = ctx->whileStatement();
  if(while_ctx) {
    this->visitWhileStatement(while_ctx);
  }

  JackParser::DoStatementContext* do_ctx = ctx->doStatement();
  if(do_ctx) {
    this->visitDoStatement(do_ctx);
  }

  JackParser::ReturnStatementContext* return_ctx = ctx->returnStatement();
  if(return_ctx) {
    this->visitReturnStatement(return_ctx);
  }

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitIfStatement(JackParser::IfStatementContext *ctx) {
  Function* F = this->Module->getFunction(this->visitorHelper.current_function_name);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* ifcond_exp = this->visitExpression(exp_ctx);
  assert(ifcond_exp && "Unable to parser if condition expression");

  // Convert condition to a bool by comparing non-equal to 0.0.
  ifcond_exp = Builder->CreateFCmpONE(ifcond_exp, ConstantFP::get(this->Context, APFloat(0.0)), "ifcond");

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  BasicBlock *ThenBB = BasicBlock::Create(this->Context, "then",    F);
  BasicBlock *ElseBB = BasicBlock::Create(this->Context, "else",    F);
  BasicBlock *MergeBB = BasicBlock::Create(this->Context, "ifcont", F);

  Builder->CreateCondBr(ifcond_exp, ThenBB, ElseBB);

  std::vector<JackParser::StatementsContext*> statements = ctx->statements();
  assert(statements.size() > 0 && "Then and Else statements cannot be both empty");
  assert(statements.size() < 3 && "If Statement cannot have more than 2 statements");
  
  // Emit then value.
  Builder->SetInsertPoint(ThenBB);
  this->visitStatements(statements[0]);
  Builder->CreateBr(MergeBB);

  // Emit else block.
  if(statements.size() == 2) {
    Builder->SetInsertPoint(ElseBB);
    this->visitStatements(statements[1]);
    Builder->CreateBr(MergeBB);
  }

  // Nothing to insert for merge block.
  Builder->SetInsertPoint(MergeBB);

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitWhileStatement(JackParser::WhileStatementContext *ctx) {
  Function* F = this->Module->getFunction(this->visitorHelper.current_function_name);
  BasicBlock *CondBB = BasicBlock::Create(this->Context, "whilecond", F);
  BasicBlock *WhileBB = BasicBlock::Create(this->Context, "while",    F);
  BasicBlock *NextBB = BasicBlock::Create(this->Context, "next", F);

  Builder->SetInsertPoint(CondBB);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* whilecond_exp = this->visitExpression(exp_ctx);
  assert(whilecond_exp && "Unable to parser while condition expression");

  // Convert condition to a bool by comparing non-equal to 0.0.
  whilecond_exp = Builder->CreateFCmpONE(whilecond_exp, ConstantFP::get(this->Context, APFloat(0.0)), "whilecond");

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.

  Builder->CreateCondBr(whilecond_exp, WhileBB, NextBB);

  // Emit while value.
  Builder->SetInsertPoint(WhileBB);
  this->visitStatements(ctx->statements());
  Builder->CreateBr(CondBB);

  // Emit next block.
  Builder->SetInsertPoint(NextBB);

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitDoStatement(JackParser::DoStatementContext *ctx) {
  this->visitSubroutineCall(ctx->subroutineCall());
  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitReturnStatement(JackParser::ReturnStatementContext *ctx) {
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  if(exp_ctx) {
    Value* return_exp = this->visitExpression(exp_ctx);
    Builder->CreateRet(return_exp);

  } else {
    Builder->CreateRetVoid();
  }

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitLetStatement(JackParser::LetStatementContext *ctx) {
  Function* F = this->Module->getFunction(this->visitorHelper.current_function_name);
  std::string var_name = this->visitVarName(ctx->varName());
  std::vector<JackParser::ExpressionContext*> exp_ctxs = ctx->expression();
  assert(exp_ctxs.size() > 0 && "Let statement has to have at least right value");
  assert(exp_ctxs.size() < 3 && "Let statement can only have at most 2 expressions");

  llvm::Value* r_val = this->visitExpression(exp_ctxs.back());
  llvm::Value* var_addr = this->variableLookup(var_name);
  if(exp_ctxs.size() == 2) {
    // VarName[exp] = exp
    // Use GEP Inst
    llvm::Value* index = this->visitExpression(exp_ctxs.front());
    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = index; // Get indexed member
    
    llvm::Value* member_addr = Builder->CreateGEP(var_addr, indices, "memberaddr");
    Builder->CreateStore(r_val, member_addr);
    
  } else {
    // VarName = exp
    // Use store Inst
    Builder->CreateStore(r_val, var_addr);
    
  }

  return nullptr;
}
