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

  JackParser::PutsStatementContext* puts_ctx = ctx->putsStatement();
  if(puts_ctx) {
    this->visitPutsStatement(puts_ctx);
  }

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitIfStatement(JackParser::IfStatementContext *ctx) {
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* ifcond_exp = this->visitExpression(exp_ctx).as<Value*>();
  assert(ifcond_exp && "Unable to parser if condition expression");

  VLOG(6) << "---- Parsing If Statement ----"; 

  // Convert condition to a bool by comparing non-equal to 0.0.
  auto& builder = getBuilder();
  Type* ifcond_exp_type = ifcond_exp->getType();
  
  // Check if cond_expression is boolean
  assert(ifcond_exp_type->isIntegerTy(1) && "Condition expression has to be boolean");
  
  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  BasicBlock* block_before = this->visitorHelper.GetBlockBefore();
  BasicBlock *ThenBB = BasicBlock::Create(getContext(), "then",    F, block_before);
  BasicBlock *ElseBB = BasicBlock::Create(getContext(), "else",    F, block_before);
  BasicBlock *MergeBB = BasicBlock::Create(getContext(), "ifcont", F, block_before);

  builder.CreateCondBr(ifcond_exp, ThenBB, ElseBB);

  std::vector<JackParser::StatementsContext*> statements = ctx->statements();
  assert(statements.size() > 0 && "Then and Else statements cannot be both empty");
  assert(statements.size() < 3 && "If Statement cannot have more than 2 statements");
  
  // Emit then value.
  builder.SetInsertPoint(ThenBB);
  this->visitorHelper.SetBlockBefore(ElseBB);
  this->visitStatements(statements[0]);
  
  builder.CreateBr(MergeBB);
  this->visitorHelper.PopBlockBefore();

  // Emit else block.
  if(statements.size() == 2) {
    builder.SetInsertPoint(ElseBB);
    this->visitorHelper.SetBlockBefore(MergeBB);

    this->visitStatements(statements[1]);
    builder.CreateBr(MergeBB);

    this->visitorHelper.PopBlockBefore();
  }

  // Nothing to insert for merge block.
  builder.SetInsertPoint(MergeBB);

  VLOG(6) << "---- Finished Parsing If Statement ----"; 
  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitWhileStatement(JackParser::WhileStatementContext *ctx) {
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  BasicBlock* block_before = this->visitorHelper.GetBlockBefore();
  BasicBlock *CondBB = BasicBlock::Create(getContext(), "whilecond", F, block_before);
  BasicBlock *WhileBB = BasicBlock::Create(getContext(), "while",    F, block_before);
  BasicBlock *NextBB = BasicBlock::Create(getContext(), "next", F, block_before);
  
  VLOG(6) << "---- Parsing While Statement ----"; 
 
  auto& builder = getBuilder();
  builder.SetInsertPoint(CondBB);
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  Value* whilecond_exp = this->visitExpression(exp_ctx).as<Value*>();
  assert(whilecond_exp && "Unable to parser while condition expression");
  
  // Check if cond_expression is boolean
  Type* cond_exp_type = whilecond_exp->getType();
  assert(cond_exp_type->isIntegerTy(1) && "Condition expression has to be boolean");

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.

  builder.CreateCondBr(whilecond_exp, WhileBB, NextBB);

  // Emit while value.
  builder.SetInsertPoint(WhileBB);
  this->visitorHelper.SetBlockBefore(NextBB);

  this->visitStatements(ctx->statements());
  builder.CreateBr(CondBB);
  
  this->visitorHelper.PopBlockBefore();

  // Emit next block.
  builder.SetInsertPoint(NextBB);

  VLOG(6) << "---- Finished Parsing While Statement ----"; 
  
  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitDoStatement(JackParser::DoStatementContext *ctx) {
  VLOG(6) << "---- Parsing Do Statement ----"; 
  
  this->visitSubroutineCall(ctx->subroutineCall());
  
  VLOG(6) << "---- Finished Parsing Do Statement ----"; 
  
  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitReturnStatement(JackParser::ReturnStatementContext *ctx) {
  VLOG(6) << "---- Parsing Return Statement ----"; 
  
  JackParser::ExpressionContext* exp_ctx = ctx->expression();
  auto& builder = getBuilder();
  if(exp_ctx) {
    Value* return_exp = this->visitExpression(exp_ctx).as<Value*>();
    builder.CreateRet(return_exp);

  } else {
    builder.CreateRetVoid();
  }
  
  VLOG(6) << "---- Finished Parsing Return Statement ----"; 

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitLetStatement(JackParser::LetStatementContext *ctx) {
  Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  std::string var_name = this->visitVarName(ctx->varName()).as<std::string>();
  std::vector<JackParser::ExpressionContext*> exp_ctxs = ctx->expression();
  assert(exp_ctxs.size() > 0 && "Let statement has to have at least right value");
  assert(exp_ctxs.size() < 3 && "Let statement can only have at most 2 expressions");
  
  VLOG(6) << "---- Parsing Let Statement ----"; 

  llvm::Value* r_val = this->visitExpression(exp_ctxs.back()).as<llvm::Value*>();

  llvm::Value* var_addr = this->variableLookup(var_name);
  auto& builder = getBuilder();
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
  
  VLOG(6) << "---- Finished Parsing Let Statement ----"; 

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitPutsStatement(JackParser::PutsStatementContext *ctx) {
  antlr4::tree::TerminalNode* string_constant_ctx = ctx->STRING();
  antlr4::Token* string_tok = string_constant_ctx->getSymbol();
  std::string string = string_tok->getText();
  auto& builder = getBuilder();

  llvm::Value* string_val = builder.CreateGlobalString(string); // [n x i8]*
  llvm::Type* int8_ptr_type = llvm::PointerType::get(llvm::Type::getInt8Ty(getContext()), 0);
  llvm::Value* int8_ptr_val = builder.CreateBitOrPointerCast(string_val, int8_ptr_type); // i8*

  std::vector<llvm::Value*> Args = { int8_ptr_val };
  
  llvm::Function* F = getModule().getFunction("puts");
  
  builder.CreateCall(F, Args);

  VLOG(6) << "Parsing Puts Statement with string : " << string;

  return nullptr;
}
