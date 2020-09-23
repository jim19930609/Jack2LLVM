#include "JackRealVisitor.h"
#include "llvm/IR/Type.h"

antlrcpp::Any JackRealVisitor::visitExpression(JackParser::ExpressionContext *ctx) {
  std::vector<antlr4::tree::TerminalNode*> op_list = ctx->OP();
  std::vector<JackParser::TermContext*> term_list = ctx->term();

  assert(term_list.size() > 0 && "Expression contains zero terms");
  assert(term_list.size() == op_list.size() + 1 && "Expression number of terms mismatch number of ops");

  llvm::Value* LHS = this->visitTerm(term_list[0]);
  for(size_t i=0; i<op_list.size(); i++) {
    llvm::Value* RHS = this->visitTerm(term_list[i+1]);
    
    // Check LHS & RHS having the same datatype
    llvm::Type* lhs_type = LHS->getType();
    llvm::Type* rhs_type = RHS->getType();
    assert(lhs_type == rhs_type && "Operation operands have different data types");

    antlr4::Token* op_tok = op_list[i]->getSymbol();
    std::string op_text = op_tok->getText();
    if(op_text == "+") {
      LHS = Builder->CreateAdd(LHS, RHS, "add");
    } else if(op_text == "-") {
      LHS = Builder->CreateSub(LHS, RHS, "sub");
    } else if(op_text == "*") {
      LHS = Builder->CreateMul(LHS, RHS, "mul");
    } else if(op_text == "/") {
      LHS = Builder->CreateSDiv(LHS, RHS, "div");
    } else if(op_text == "&") {
      LHS = Builder->CreateAnd(LHS, RHS, "and");
    } else if(op_text == "|") {
      LHS = Builder->CreateOr(LHS, RHS, "or");
    } else if(op_text == ">") {
      LHS = Builder->CreateICmpSGT(LHS, RHS, "gt");
    } else if(op_text == "<") {
      LHS = Builder->CreateICmpSLT(LHS, RHS, "lt");
    } else if(op_text == "=") {
      LHS = Builder->CreateICmpEQ(LHS, RHS, "eq");
    } else {
      assert(false && "Unrecognized operation type");
    }
  }

  return LHS;
}

antlrcpp::Any JackRealVisitor::visitTerm(JackParser::TermContext *ctx) {
  // All possible members
  antlr4::tree::TerminalNode* integer_constant_ctx = ctx->INTEGER();
  antlr4::tree::TerminalNode* string_constant_ctx = ctx->STRING();
  antlr4::tree::TerminalNode* keyword_constant_ctx = ctx->KEYWORD();
  std::vector<JackParser::VarNameContext*> var_name_ctxs = ctx->varName();
  JackParser::ExpressionContext* expression_ctx = ctx->expression();
  JackParser::SubroutineCallContext* subroutine_ctx = ctx->subroutineCall();
  antlr4::tree::TerminalNode* unary_op_ctx = ctx->UNARYOP();
  JackParser::TermContext* term_ctx = ctx->term();
  
  // intergerConstant
  if(integer_constant_ctx) {
    antlr4::Token* integer_tok = integer_constant_ctx->getSymbol();
    int integer = std::atoi(integer_tok->getText().c_str());
    llvm::Value* integer_val = llvm::ConstantInt::get(this->Context, llvm::APInt(32, integer, true));

    return integer_val;
  }
  // stringConstant
  if(string_constant_ctx) {
    antlr4::Token* string_tok = string_constant_ctx->getSymbol();
    std::string string = string_tok->getText();
    llvm::Value* string_val = Builder->CreateGlobalString(string);
  
    return string_val;
  }
  
  // keywordConstant
  if(keyword_constant_ctx) {
    antlr4::Token* keyword_tok = keyword_constant_ctx->getSymbol();
    std::string keyword = keyword_tok->getText();

    if(keyword == "true"){
      return llvm::APInt(1, 1, true);
    
    } else if(keyword == "false") {
      return llvm::APInt(1, 0, true);
    
    } else if(keyword == "null") {
      return llvm::ConstantPointerNull::get(llvm::Type::getInt32PtrTy(this->Context));
    
    } else if(keyword == "this") {
      assert(this->visitorHelper.symtab_a.count("this") && "Unable to find 'this' in symbol table");
      return this->visitorHelper.symtab_a["this"];
    
    } else {
      assert(false && "Unrecognized keyword constant");
    }
  }

  // varName.varName
  if(var_name_ctxs.size() == 2) {
    std::string class_var_name = this->visitVarName(var_name_ctxs[0]);
    std::string member_var_name = this->visitVarName(var_name_ctxs[1]);

    llvm::Value* class_var_addr = variableLookup(class_var_name);
    llvm::Type* class_type = class_var_addr->getType();

    size_t index = this->visitorHelper.class_member_name_to_index[class_type][member_var_name];
    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, index, true)); // Get indexed member

    llvm::Value* member_addr = Builder->CreateGEP(class_var_addr, indices, "class_member_addr");
    return Builder->CreateLoad(member_addr, "loadvalue");
  }

  assert(var_name_ctxs.size() == 1 && "Invalid number of VarNames found in Term");
  JackParser::VarNameContext* var_name_ctx = var_name_ctxs[0];
  
  // varName[expression]
  if(var_name_ctx && expression_ctx) {
    std::string var_name = this->visitVarName(var_name_ctx);
    llvm::Value* var_addr = variableLookup(var_name);
    llvm::Value* index = this->visitExpression(expression_ctx);

    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = index; // Get indexed member

    llvm::Value* member_addr = Builder->CreateGEP(var_addr, indices, "array_member_addr");
    return Builder->CreateLoad(member_addr, "loadvalue");
  }
  
  // varName
  if(var_name_ctx) {
    std::string var_name = this->visitVarName(var_name_ctx);
    return variableLookup(var_name);
  }
  
  // (expression)
  if(expression_ctx) {
    return this->visitExpression(expression_ctx);
  }

  // subroutineCall
  if(subroutine_ctx) {
    return this->visitSubroutineCall(subroutine_ctx);
  }
  
  // unaryOp term
  if(unary_op_ctx && term_ctx) {
    antlr4::Token* unary_op_tok = unary_op_ctx->getSymbol();
    std::string unary_op_text = unary_op_tok->getText();
    llvm::Value* term_val = this->visitTerm(term_ctx);
    if(unary_op_text == "-") {
      return Builder->CreateNeg(term_val, "neg");
    } else if(unary_op_text == "~") {
      return Builder->CreateNot(term_val, "not");
    } else {
      assert(false && "Unrecognized unary op");
    }
  }

  assert(false && "Unrecognized Term");
}

antlrcpp::Any JackRealVisitor::visitSubroutineCall(JackParser::SubroutineCallContext *ctx) {
    JackParser::ExpressionListContext* expressionlist_ctx = ctx->expressionList();
    JackParser::SubroutineNameContext* subroutine_name_ctx = ctx->subroutineName();
    JackParser::ClassNameContext* class_name_ctx = ctx->className();
    JackParser::VarNameContext* var_name_ctx = ctx->varName();

    assert(subroutine_name_ctx && "No subroutine name specified");

    // Parse ExpressionList to create Args
    std::vector<llvm::Value*> Args = this->visitExpressionList(expressionlist_ctx);

    // Parse FunctionName to get Function
    std::string function_name = this->visitSubroutineName(subroutine_name_ctx);
    std::string function_name_mangled;
    if(class_name_ctx) {
      // "function": static function call
      std::string class_name = this->visitClassName(class_name_ctx);
      // Get class type
      llvm::Type* class_type = this->Module->getTypeByName(class_name);
      function_name_mangled = this->visitorHelper.static_func_name_mapping[class_type][function_name];
      
      llvm::Function* F = this->Module->getFunction(function_name_mangled);
      return Builder->CreateCall(F, Args, "call");

    } else if(var_name_ctx) {
      // "method": member function call
      std::string var_name = this->visitVarName(var_name_ctx);

      // Get class type
      llvm::Value* var_value = variableLookup(var_name);
      llvm::Type*  class_type = var_value->getType();
      assert(class_type->isStructTy() && "Variable is not callable");

      // Get Function from vtable
      // First locate vtable
      size_t vtable_index = class_type->getStructNumElements() - 1;
      std::vector<std::string>& vtable_function_order = this->visitorHelper.class_vtable_function_order[class_type];
      size_t function_index = std::find(vtable_function_order.begin(), vtable_function_order.begin(), function_name) - vtable_function_order.end();

      // Index has 3 levels:
      // 1. Pointer itself
      // 2. Vtable addr in this_type
      // 3. Function index in Vtable
      std::vector<llvm::Value*> indices(3);
      indices[0] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, 0, true)); // Get the pointer itself
      indices[1] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, vtable_index, true));
      indices[2] = llvm::ConstantInt::get(this->Context, llvm::APInt(32, function_index, true));
      // This is the function pointer
      llvm::Value* member_addr = Builder->CreateGEP(var_value, indices, "function_addr_in_vtable");
      llvm::Value* member_function = Builder->CreateLoad(member_addr, "Load function ptr");

      // Virtual functions should have the same function type
      // We just grab it here
      function_name_mangled = this->visitorHelper.class_func_name_mapping[class_type][function_name];
      llvm::FunctionType* function_type = this->Module->getFunction(function_name_mangled)->getFunctionType();

      return Builder->CreateCall(function_type, member_function, Args, "call");
    }
    
    return nullptr;    
}

antlrcpp::Any JackRealVisitor::visitExpressionList(JackParser::ExpressionListContext *ctx) {
  std::vector<llvm::Value*> ret;
  std::vector<JackParser::ExpressionContext*> expression_ctxs = ctx->expression();
  for(JackParser::ExpressionContext* expression_ctx : expression_ctxs) {
    ret.emplace_back(this->visitExpression(expression_ctx));
  }
  return ret;
}
