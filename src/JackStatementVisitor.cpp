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
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* ifcond_exp = this->visitExpression(exp_ctx).as<Value*>();
  assert(ifcond_exp && "Unable to parser if condition expression");

  // Convert condition to a bool by comparing non-equal to 0.0.
  auto builder = getBuilder();
  ifcond_exp = builder.CreateFCmpONE(ifcond_exp, ConstantFP::get(getContext(), APFloat(0.0)), "ifcond");

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  BasicBlock *ThenBB = BasicBlock::Create(getContext(), "then",    F);
  BasicBlock *ElseBB = BasicBlock::Create(getContext(), "else",    F);
  BasicBlock *MergeBB = BasicBlock::Create(getContext(), "ifcont", F);

  builder.CreateCondBr(ifcond_exp, ThenBB, ElseBB);

  std::vector<JackParser::StatementsContext*> statements = ctx->statements();
  assert(statements.size() > 0 && "Then and Else statements cannot be both empty");
  assert(statements.size() < 3 && "If Statement cannot have more than 2 statements");
  
  // Emit then value.
  builder.SetInsertPoint(ThenBB);
  this->visitStatements(statements[0]);
  builder.CreateBr(MergeBB);

  // Emit else block.
  if(statements.size() == 2) {
    builder.SetInsertPoint(ElseBB);
    this->visitStatements(statements[1]);
    builder.CreateBr(MergeBB);
  }

  // Nothing to insert for merge block.
  builder.SetInsertPoint(MergeBB);

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitWhileStatement(JackParser::WhileStatementContext *ctx) {
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  BasicBlock *CondBB = BasicBlock::Create(getContext(), "whilecond", F);
  BasicBlock *WhileBB = BasicBlock::Create(getContext(), "while",    F);
  BasicBlock *NextBB = BasicBlock::Create(getContext(), "next", F);
 
  auto builder = getBuilder();
  builder.SetInsertPoint(CondBB);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* whilecond_exp = this->visitExpression(exp_ctx).as<Value*>();
  assert(whilecond_exp && "Unable to parser while condition expression");

  // Convert condition to a bool by comparing non-equal to 0.0.
  whilecond_exp = builder.CreateFCmpONE(whilecond_exp, ConstantFP::get(getContext(), APFloat(0.0)), "whilecond");

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.

  builder.CreateCondBr(whilecond_exp, WhileBB, NextBB);

  // Emit while value.
  builder.SetInsertPoint(WhileBB);
  this->visitStatements(ctx->statements());
  builder.CreateBr(CondBB);

  // Emit next block.
  builder.SetInsertPoint(NextBB);

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitDoStatement(JackParser::DoStatementContext *ctx) {
  this->visitSubroutineCall(ctx->subroutineCall());
  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitReturnStatement(JackParser::ReturnStatementContext *ctx) {
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  auto builder = getBuilder();
  if(exp_ctx) {
    Value* return_exp = this->visitExpression(exp_ctx).as<Value*>();
    builder.CreateRet(return_exp);

  } else {
    builder.CreateRetVoid();
  }

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitLetStatement(JackParser::LetStatementContext *ctx) {
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  std::string var_name = this->visitVarName(ctx->varName()).as<std::string>();
  std::vector<JackParser::ExpressionContext*> exp_ctxs = ctx->expression();
  assert(exp_ctxs.size() > 0 && "Let statement has to have at least right value");
  assert(exp_ctxs.size() < 3 && "Let statement can only have at most 2 expressions");

  llvm::Value* r_val = this->visitExpression(exp_ctxs.back()).as<llvm::Value*>();
  llvm::Value* var_addr = this->variableLookup(var_name);
  auto builder = getBuilder();
  if(exp_ctxs.size() == 2) {
    // VarName[exp] = exp
    // Use GEP Inst
    llvm::Value* index = this->visitExpression(exp_ctxs.front()).as<llvm::Value*>();
    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = index; // Get indexed member
    
    llvm::Value* member_addr = builder.CreateGEP(var_addr, indices, "memberaddr");
    builder.CreateStore(r_val, member_addr);
    
  } else {
    // VarName = exp
    // Use store Inst
    builder.CreateStore(r_val, var_addr);
    
  }

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitCastStatement(JackParser::CastStatementContext *ctx) {
  std::string var_name = this->visitVarName(ctx->varName()).as<std::string>();
  llvm::Value* var_addr = this->variableLookup(var_name);
  
  llvm::Type* srcType = var_addr->getType();
  llvm::Type* dstType = this->visitType(ctx->type()).as<llvm::Type*>();

  auto builder = getBuilder();
  if(srcType->isIntegerTy() && dstType->isIntegerTy()) {
    llvm::Value* var_val = builder.CreateLoad(var_addr, "load_for_cast");
    llvm::Value* casted_val = builder.CreateIntCast(var_val, dstType, true, "basic_type_cast");
    builder.CreateStore(casted_val, var_addr);

  } else if(srcType->isStructTy() && dstType->isStructTy()) {
    builder.CreatePointerCast(var_addr, dstType, "struct_type_cast");

  } else {
    assert(false && "Src and Dst of cast statement has to be both basic type or both ptr types, cannot do mix");
  }
  
  return nullptr;
}
