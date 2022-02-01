#include "JackRealVisitor.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"

antlrcpp::Any JackRealVisitor::visitExpression(JackParser::ExpressionContext *ctx) {
  std::vector<antlr4::tree::TerminalNode*> op_list = ctx->OP();
  std::vector<JackParser::TermContext*> term_list = ctx->term();

  VLOG(6) << "---- Parsing Expression ----";

  assert(term_list.size() > 0 && "Expression contains zero terms");
  assert(term_list.size() == op_list.size() + 1 && "Expression number of terms mismatch number of ops");

  llvm::Value* LHS = this->visitTerm(term_list[0]).as<llvm::Value*>();
  auto& builder = getBuilder();
  for(size_t i=0; i<op_list.size(); i++) {
    llvm::Value* RHS = this->visitTerm(term_list[i+1]).as<llvm::Value*>();
    
    // Check LHS & RHS having the same datatype
    llvm::Type* lhs_type = LHS->getType();
    llvm::Type* rhs_type = RHS->getType();

    assert(lhs_type == rhs_type && "LHS & RHS should have the same datatype");

    antlr4::Token* op_tok = op_list[i]->getSymbol();
    std::string op_text = op_tok->getText();
    if(op_text == "+") {
      LHS = builder.CreateAdd(LHS, RHS, "add");
    } else if(op_text == "-") {
      LHS = builder.CreateSub(LHS, RHS, "sub");
    } else if(op_text == "*") {
      LHS = builder.CreateMul(LHS, RHS, "mul");
    } else if(op_text == "/") {
      LHS = builder.CreateSDiv(LHS, RHS, "div");
    } else if(op_text == "&") {
      LHS = builder.CreateAnd(LHS, RHS, "and");
    } else if(op_text == "|") {
      LHS = builder.CreateOr(LHS, RHS, "or");
    } else if(op_text == ">") {
      LHS = builder.CreateICmpSGT(LHS, RHS, "gt");
    } else if(op_text == "<") {
      LHS = builder.CreateICmpSLT(LHS, RHS, "lt");
    } else if(op_text == "=") {
      LHS = builder.CreateICmpEQ(LHS, RHS, "eq");
    } else {
      assert(false && "Unrecognized operation type");
    }
  }
  
  VLOG(6) << "Expression Parsing Result";
  print_llvm_type(LHS->getType());

  VLOG(6) << "---- Finished Parsing Expression ----";
  
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
    
  auto& builder = getBuilder();
  
  // intergerConstant
  if(integer_constant_ctx) {
    antlr4::Token* integer_tok = integer_constant_ctx->getSymbol();
    int integer = std::atoi(integer_tok->getText().c_str());
    llvm::Value* integer_val = llvm::ConstantInt::get(getContext(), llvm::APInt(32, integer, true));
    
    VLOG(6) << "Parsing Integer Term : " << integer;

    return integer_val;
  }
  // stringConstant
  if(string_constant_ctx) {
    antlr4::Token* string_tok = string_constant_ctx->getSymbol();
    std::string string = string_tok->getText();
    llvm::Value* string_val = builder.CreateGlobalString(string);
    
    VLOG(6) << "Parsing String Term : " << string;
  
    return string_val;
  }
  
  // keywordConstant
  if(keyword_constant_ctx) {
    antlr4::Token* keyword_tok = keyword_constant_ctx->getSymbol();
    std::string keyword = keyword_tok->getText();
    
    VLOG(6) << "Parsing Keyword Term : " << keyword;

    if(keyword == "true"){
      llvm::Value* return_val = llvm::ConstantInt::getTrue(getContext());
      return return_val;
    
    } else if(keyword == "false") {
      llvm::Value* return_val = llvm::ConstantInt::getFalse(getContext());
      return return_val;
    
    } else if(keyword == "null") {
      llvm::Value* return_val = llvm::ConstantPointerNull::get(llvm::Type::getInt32PtrTy(getContext()));
      return return_val;
    
    } else if(keyword == "this") {
      llvm::Value* return_val = variableLookup("this");
      return return_val;
    
    } else {
      assert(false && "Unrecognized keyword constant");
    }
  }

  // varName.varName
  if(var_name_ctxs.size() == 2) {
    std::string class_var_name = this->visitVarName(var_name_ctxs[0]).as<std::string>();
    std::string member_var_name = this->visitVarName(var_name_ctxs[1]).as<std::string>();

    llvm::Value* class_var_addr = variableLookup(class_var_name);
    llvm::Type* class_type = class_var_addr->getType();

    size_t index = this->visitorHelper.class_member_name_to_index[class_type][member_var_name];
    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, index, true)); // Get indexed member

    llvm::Value* member_addr = builder.CreateGEP(class_var_addr, indices, "class_member_addr");
    
    VLOG(6) << "Parsing VarName.VarName Term : " << class_var_name << "." << member_var_name;
    
    llvm::Value* member_val = builder.CreateLoad(member_addr, "loadvalue");
    return member_val;
  }

  assert(var_name_ctxs.size() == 1 && "Invalid number of VarNames found in Term");
  JackParser::VarNameContext* var_name_ctx = var_name_ctxs[0];
  
  // varName[expression]
  if(var_name_ctx && expression_ctx) {
    std::string var_name = this->visitVarName(var_name_ctx).as<std::string>();
    llvm::Value* var_addr = variableLookup(var_name);
    llvm::Value* index = this->visitExpression(expression_ctx).as<llvm::Value*>();

    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = index; // Get indexed member

    llvm::Value* member_addr = builder.CreateGEP(var_addr, indices, "array_member_addr");
    
    VLOG(6) << "Parsing VarName[expression] Term : " << var_name;
    llvm::Value* member_val = builder.CreateLoad(member_addr, "loadvalue");
    return member_val;
  }
  
  // varName
  if(var_name_ctx) {
    std::string var_name = this->visitVarName(var_name_ctx).as<std::string>();
    VLOG(6) << "Parsing VarName Term : " << var_name;
    llvm::Value* var_addr = variableLookup(var_name);
    llvm::Value* var_val = builder.CreateLoad(var_addr, "loadvalue");
    return var_val;
  }
  
  // (expression)
  if(expression_ctx) {
    VLOG(6) << "Parsing Expression Term";
    return this->visitExpression(expression_ctx);
  }

  // subroutineCall
  if(subroutine_ctx) {
    VLOG(6) << "Parsing SubroutineCall Term";
    return this->visitSubroutineCall(subroutine_ctx);
  }
  
  // unaryOp term
  if(unary_op_ctx && term_ctx) {
    antlr4::Token* unary_op_tok = unary_op_ctx->getSymbol();
    std::string unary_op_text = unary_op_tok->getText();
    llvm::Value* term_val = this->visitTerm(term_ctx).as<llvm::Value*>();

    VLOG(6) << "Parsed unaryOp Term: " << unary_op_text;

    if(unary_op_text == "-") {
      llvm::Value* return_val = builder.CreateNeg(term_val, "neg");
      return return_val;
    } else if(unary_op_text == "~") {
      llvm::Value* return_val = builder.CreateNot(term_val, "not");
      return return_val;
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
    std::vector<llvm::Value*> Args = this->visitExpressionList(expressionlist_ctx).as<std::vector<llvm::Value*>>();

    // Parse FunctionName to get Function
    std::string function_name = this->visitSubroutineName(subroutine_name_ctx).as<std::string>();
    std::string function_name_mangled;
    auto& builder = getBuilder();
    if(class_name_ctx) {
      // "function": static function call
      std::string class_name = this->visitClassName(class_name_ctx).as<std::string>();
      // Get class type
      llvm::Module& module = getModule();
      llvm::Type* class_type = module.getTypeByName(class_name);
      function_name_mangled = this->visitorHelper.static_func_name_mapping[class_type][function_name];
      
      llvm::Function* F = module.getFunction(function_name_mangled);

      VLOG(6) << "Detected Subroutine Call";
      print_llvm_type(F->getType());

      return builder.CreateCall(F, Args, "call");

    } else if(var_name_ctx) {
      // "method": member function call
      std::string var_name = this->visitVarName(var_name_ctx).as<std::string>();

      // Get class type
      llvm::Value* var_addr = variableLookup(var_name);
      llvm::Value* var_value = builder.CreateLoad(var_addr, "loadvalue");

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
      indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
      indices[1] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, vtable_index, true));
      indices[2] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, function_index, true));
      // This is the function pointer
      llvm::Value* member_addr = builder.CreateGEP(var_value, indices, "function_addr_in_vtable");
      llvm::Value* member_function = builder.CreateLoad(member_addr, "Load function ptr");

      // Virtual functions should have the same function type
      // We just grab it here
      function_name_mangled = this->visitorHelper.class_func_name_mapping[class_type][function_name];
      llvm::Module& module = getModule();
      llvm::FunctionType* function_type = module.getFunction(function_name_mangled)->getFunctionType();
      
      VLOG(6) << "Detected Subroutine Call";
      print_llvm_type(function_type);

      return builder.CreateCall(function_type, member_function, Args, "call");
    }
    
    return nullptr;    
}

antlrcpp::Any JackRealVisitor::visitExpressionList(JackParser::ExpressionListContext *ctx) {
  std::vector<llvm::Value*> ret;
  std::vector<JackParser::ExpressionContext*> expression_ctxs = ctx->expression();
  for(JackParser::ExpressionContext* expression_ctx : expression_ctxs) {
    ret.emplace_back(this->visitExpression(expression_ctx).as<llvm::Value*>());
  }
  return ret;
}
