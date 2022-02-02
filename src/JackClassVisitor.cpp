
// Generated from Jack.g4 by ANTLR 4.8

#include "JackRealVisitor.h"
#include "llvm/IR/Type.h"

antlrcpp::Any JackRealVisitor::visitClassDec(JackParser::ClassDecContext *ctx) {
  
  // Class Name
  std::vector<JackParser::ClassNameContext*> class_name_ctxs = ctx->className();
  assert(class_name_ctxs.size() <= 2 && "Too many class names detected");
  
  JackParser::ClassNameContext* class_name_ctx = class_name_ctxs[0];
  std::string class_name_text = this->visitClassName(class_name_ctx).as<std::string>();
  
  VLOG(6) << "------ Visiting ClassDec : " << class_name_text << "  ------";

  // Handle Inheritance if there is any
  // Parent's members/functions are registered first
  // Then override by that from curent class
  //
  std::vector<llvm::Type*> struct_members;
  std::unordered_map<std::string, std::string> globalvar_name_mapping;
  std::unordered_map<std::string, std::string> func_name_mapping;
  std::unordered_map<std::string, std::string> static_func_name_mapping;
  std::vector<std::string> vtable_function_order;
  this->visitorHelper.symtab_c.clear();
  if(class_name_ctxs.size() == 2) {
    VLOG(6) << "Handling Inheritance";

    JackParser::ClassNameContext* parent_class_name_ctx = class_name_ctxs[1];
    std::string parent_class_name = this->visitClassName(parent_class_name_ctx).as<std::string>();
    
    // Member variable
    llvm::StructType* parent_type = getModule().getTypeByName(parent_class_name);
    for(size_t i=0; i<parent_type->getNumElements(); i++) {
      llvm::Type* member_type = parent_type->getTypeAtIndex(i);
      std::string member_name;
      // kv.first: name
      // kv.second: index
      for(auto& kv : this->visitorHelper.class_member_name_to_index[parent_type]) {
        if(kv.second == i) {
          member_name = kv.first;
          break;
        }
      }

      struct_members.push_back(member_type);
      this->visitorHelper.symtab_c[member_name] = i;
    }

    VLOG(6) << "Handled Parent Members";
    
    // Functions and Static members
    // Init with parent's mapping
    globalvar_name_mapping = this->visitorHelper.class_globalvar_name_mapping[parent_type];
    func_name_mapping = this->visitorHelper.class_func_name_mapping[parent_type];
    static_func_name_mapping = this->visitorHelper.static_func_name_mapping[parent_type];
    vtable_function_order = this->visitorHelper.class_vtable_function_order[parent_type];
    
    VLOG(6) << "Handled Parent Methods";
  }


  VLOG(6) << "Class Name: " << class_name_ctx;

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
    
    std::vector<std::pair<std::string, llvm::Type*>> class_vars = this->visitClassVarDec(class_var_dec_ctx).as<std::vector<std::pair<std::string, llvm::Type*>>>();
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
      VLOG(6) << "Parsed Class Var: " << decorator << " " << class_vars[class_var_index].first;
    }
  }
  
  // ---------- //
  // Field Vars //
  // ---------- //
  // register a struct type in llvm::Module
  for(size_t i=0; i<field_class_vars.size(); i++) {
    std::string name = field_class_vars[i].first;
    llvm::Type*  type = field_class_vars[i].second;
    // Declare Struct in Module
    size_t index = struct_members.size();
    struct_members.push_back(type);
    // Add to symbol table
    // Consider parent members
    this->visitorHelper.symtab_c[name] = index;
  }

  VLOG(6) << "Registered Class-level Field Vars";

  // Insert virtual table to the end
  // Virtual table will be a struct containing pointer (i64 format) casted from Function*
  size_t num_subroutines = ctx->subroutineDec().size();
  llvm::ArrayType* vtable = llvm::ArrayType::get(llvm::Type::getInt64Ty(getContext()), num_subroutines);
  this->visitorHelper.symtab_c["_vtable"] = struct_members.size();
  struct_members.push_back(vtable);
  
  llvm::StructType* registered_class_type = llvm::StructType::create(getContext(), struct_members, class_name_text, true);
  assert(registered_class_type && "Unable to create class StructType during ClassDec");
  
  VLOG(6) << "Handled Virtual Table";

  // Copy symtab_c
  this->visitorHelper.class_member_name_to_index[registered_class_type] = this->visitorHelper.symtab_c;

  // ----------- //
  // Static Vars //
  // ----------- //
  // register a global var with class_name prefix in LLVM::Module
  for(const auto& kv : static_class_vars) {

    llvm::Type* global_type = kv.second;
    std::string global_name = kv.first;
    
    // Child cannot override parent's static member
    assert(globalvar_name_mapping.count(global_name) == 0 && "Overriding parent's static member is illegal");
    
    // Apply special prefix to static member var
    // Then make it a global var
    std::string global_name_mangled = class_name_text + "." + global_name;
    llvm::Constant* declared_global_var = getModule().getOrInsertGlobal(global_name_mangled, global_type);
    assert(declared_global_var && "Unable to register static member as global variable");
  
    // Update global var names mapping
    globalvar_name_mapping[global_name] = global_name_mangled;
  }

  VLOG(6) << "Registered Static Vars";

  this->visitorHelper.current_class_name = class_name_text;
  this->visitorHelper.class_globalvar_name_mapping[registered_class_type] = globalvar_name_mapping;
  this->visitorHelper.class_func_name_mapping[registered_class_type] = func_name_mapping;
  this->visitorHelper.static_func_name_mapping[registered_class_type] = static_func_name_mapping;
  this->visitorHelper.class_vtable_function_order[registered_class_type] = vtable_function_order;

  // ------------------- //
  // Parse SubroutineDec //
  // ------------------- //
  // Delay constructor registration to the end
  // Because we need to update vtable at construction
  JackParser::SubroutineDecContext* constructor_ctx = nullptr;
  std::vector<JackParser::SubroutineDecContext*> subroutine_dec_ctxs = ctx->subroutineDec();
  for(JackParser::SubroutineDecContext* subroutine_dec_ctx : subroutine_dec_ctxs) {
    // Delay constructor code gen
    antlr4::tree::TerminalNode* decorator_ctx = subroutine_dec_ctx->SUBROUTINEDECORATOR();
    antlr4::Token* decorator_tok = decorator_ctx->getSymbol();
    std::string decorator = decorator_tok->getText();
    if(decorator == "constructor") {
      constructor_ctx = subroutine_dec_ctx;
      continue;
    }

    // visitSubroutineDec() will register functions to llvm::Module in place
    this->visitSubroutineDec(subroutine_dec_ctx);

    VLOG(6) << "Parsed SubroutineDec with decorator: " << decorator;
  }

  // Constructor codegen: update vtable
  if(constructor_ctx) {
      this->visitSubroutineDec(constructor_ctx);
  }

  VLOG(6) << "------ Finished Parsing ClassDec ------";

  // Everything registered in llvm::Module
  // We dont need to return anything
  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitSubroutineDec(JackParser::SubroutineDecContext *ctx) {
  llvm::Type* this_type = getModule().getTypeByName(this->visitorHelper.current_class_name);
  // ------------------------ //
  // Parse function decorator //
  // ------------------------ //
  // 'constructor', 'function', 'method'
  // This influence the argument list and subroutineBody
  antlr4::tree::TerminalNode* subroutine_decorator = ctx->SUBROUTINEDECORATOR();
  antlr4::Token* subroutine_decorator_tok = subroutine_decorator->getSymbol();
  std::string subroutine_decorator_text = subroutine_decorator_tok->getText();
  this->visitorHelper.function_decorator = subroutine_decorator_text; 
  
  // --------------- //
  // Subroutine Name //
  // --------------- //
  JackParser::SubroutineNameContext* subroutine_name_ctx = ctx->subroutineName();
  std::string subroutine_name = this->visitSubroutineName(subroutine_name_ctx).as<std::string>();
  
  VLOG(6) << "------ Visiting SubroutineDec : " << subroutine_decorator_text << " " << subroutine_name << " ------";
  
  // Add prefix to function name
  std::string class_name = this->visitorHelper.current_class_name;
  std::string subroutine_name_mangled = class_name + "." + subroutine_name;
  if(subroutine_decorator_text == "method" || subroutine_decorator_text == "constructor") {
    // Update subroutine names mapping
    this->visitorHelper.class_func_name_mapping[this_type][subroutine_name] = subroutine_name_mangled;
  
  } else if(subroutine_decorator_text == "function") {
    if(this->visitorHelper.static_func_name_mapping[this_type].count(subroutine_name))
      assert(false && "Cannot override static function through inheritance");
    this->visitorHelper.static_func_name_mapping[this_type][subroutine_name] = subroutine_name_mangled;

  } else {
    assert(false && "subroutine type has to be method, constructor or function");
  }
  this->visitorHelper.current_function_name = subroutine_name_mangled;

  // ----------------- //
  // Parse return type //
  // ----------------- //
  // Default return type is void
  llvm::Type* return_type = llvm::Type::getVoidTy(getContext());
  JackParser::TypeContext* return_type_ctx = ctx->type();
  if(subroutine_decorator_text == "constructor") {
    // Reset Constructor's return type to 'ClassType'
    return_type = this_type;    
  } else if(return_type_ctx) {
    return_type = this->visitType(return_type_ctx).as<llvm::Type*>();
  }
  
  VLOG(6) << "Parsed Return Type";
  print_llvm_type(return_type);

  // ------------------ //
  // Construct Function //
  // ------------------ //
  // Construct parameter list
  // visitParameterList() returns vector of arguments llvm::Type
  JackParser::ParameterListContext* parameter_list_ctx = ctx->parameterList();
  std::pair<std::vector<llvm::Type*>, std::vector<std::string>> argument_list = this->visitParameterList(parameter_list_ctx).as<std::pair<std::vector<llvm::Type*>, std::vector<std::string>>>();

  std::vector<llvm::Type*> argument_type_list = argument_list.first;
  std::vector<std::string> argument_name_list = argument_list.second;

  // Insert 'this' to first argument
  if(subroutine_decorator_text == "method") {
    argument_type_list.insert(argument_type_list.begin(), this_type);
    argument_name_list.insert(argument_name_list.begin(), "this");
  }

  // Create Function
  llvm::FunctionType *FT = llvm::FunctionType::get(return_type, argument_type_list, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, subroutine_name_mangled, getModule());
  
  // Set Argument names
  size_t Idx = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(argument_name_list[Idx++]);
  }
  
  VLOG(6) << "Created Function Type";
  print_llvm_type(FT);
  VLOG(6) << "";

  // ------------------- //
  // Add Subroutine Body //
  // ------------------- //
  // Setup SubroutineBodyContext
  // and then call visitSubroutineBody()
  JackParser::SubroutineBodyContext* subroutine_body_ctx= ctx->subroutineBody();
  this->visitSubroutineBody(subroutine_body_ctx);
      
  // Constructor must insert return type
  if(subroutine_decorator_text == "constructor") {
    auto& builder = getBuilder();
    llvm::Value* this_addr = variableLookup("this");
    
    llvm::Value* this_val = builder.CreateLoad(this_addr, "load_this");
    builder.CreateRet(this_val);
  }
  
  VLOG(6) << "------ Finished Parsing SubroutineDec ------";

  return nullptr;
}


antlrcpp::Any JackRealVisitor::visitParameterList(JackParser::ParameterListContext *ctx) {
  std::vector<llvm::Type*> types;
  std::vector<std::string> names;
  
  std::vector<JackParser::TypeContext*> type_ctxs = ctx->type();
  std::vector<JackParser::VarNameContext*> var_name_ctxs = ctx->varName();
  for(size_t i=0; i<type_ctxs.size(); i++) {
    // Parse Type
    llvm::Type* argType = this->visitType(type_ctxs[i]).as<llvm::Type*>();
    types.emplace_back(argType);

    std::string var_name_str = this->visitVarName(var_name_ctxs[i]).as<std::string>();
    names.emplace_back(var_name_str);
  
    VLOG(6) << "Parsed Argument : " << var_name_str;
    print_llvm_type(argType);
    VLOG(6) << "";
  }

  return std::make_pair(types, names);
}


antlrcpp::Any JackRealVisitor::visitSubroutineBody(JackParser::SubroutineBodyContext *ctx) {
  llvm::Function* F = getModule().getFunction(this->visitorHelper.current_function_name);
  // ---------------- //
  // Parse statements //
  // ---------------- //
  // 1. Add entry BB
  VLOG(6) << "---- Parsing Subroutine Body ----";
  llvm::BasicBlock* BB = llvm::BasicBlock::Create(getContext(), "entry", F);

  auto& builder = getBuilder();
  builder.SetInsertPoint(BB);
  
  // -------------------------- //
  // Init and Contruct symtab_l //
  // -------------------------- //
  // 2. Constructor/Method/Function: Init and construct symtab_l
  this->visitorHelper.symtab_l.clear();
  std::vector<JackParser::VarDecContext*> var_dec_ctxs = ctx->varDec();
  for(const auto& var_dec_ctx : var_dec_ctxs) {
    std::vector<std::pair<std::string, llvm::Type*>> local_vars = this->visitVarDec(var_dec_ctx).as<std::vector<std::pair<std::string, llvm::Type*>>>();
    for(const auto& kv : local_vars) {
      std::string name = kv.first;
      llvm::Type* type = kv.second;
      llvm::AllocaInst* var_addr = builder.CreateAlloca(type, 0, name);
      this->visitorHelper.symtab_l[name] = var_addr;

      VLOG(6) << "Local Symbol : " << name;
      print_llvm_type(type);
      VLOG(6) << "";
    }
  }
  
  VLOG(6) << "Finished Updating Symbol Table Local";

  // ----------------------------------------- //
  // Init and Contruct symtab_l from Arguments //
  // ----------------------------------------- //
  // 2. Constructor only: allocate for self
  //    Construct virtual table
  if(this->visitorHelper.function_decorator == "constructor") {
    std::string class_name = this->visitorHelper.current_class_name;
    llvm::Type* this_type = getModule().getTypeByName(class_name);
    llvm::AllocaInst* this_addr = builder.CreateAlloca(this_type, 0, "this");
    this->visitorHelper.symtab_l["this"] = this_addr;
    
    VLOG(6) << "Handled Constructor \"this\" Argument";

    // Now update vtable
    size_t vtable_index = this->visitorHelper.symtab_c["_vtable"];
    std::unordered_map<std::string, std::string>& func_name_mapping = this->visitorHelper.class_func_name_mapping[this_type];
    std::vector<std::string>& vtable_functions = this->visitorHelper.class_vtable_function_order[this_type];
    for(auto& kv : func_name_mapping) {
      std::string name_demangled = kv.first;
      // Overrided function, keep the same position as parent
      if(std::find(vtable_functions.begin(), vtable_functions.end(), name_demangled) != vtable_functions.end()) continue;
      vtable_functions.push_back(kv.first);
    }

    for(size_t function_index=0; function_index<vtable_functions.size(); function_index++) {
      std::string function_name = vtable_functions[function_index];
      std::string function_name_mangled = func_name_mapping[function_name];
      llvm::Function* member_function = getModule().getFunction(function_name_mangled);
      
      // Index has 3 levels:
      // 1. Pointer itself
      // 2. Vtable addr in this_type
      // 3. Function index in Vtable
      std::vector<llvm::Value*> indices(3);
      indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
      indices[1] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, vtable_index, true));
      indices[2] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, function_index, true));

      llvm::Value* vtable_back_addr = builder.CreateGEP(this_addr, indices, "function_addr_in_vtable");
      builder.CreateStore(member_function, vtable_back_addr);
    }

    VLOG(6) << "Finished Constructing Vtable as Class Member";
  }

  // Construct local variables according to arguments
  // Then copy arguments value to local var
  // 'this' is added to front of arguments during call/constructor
  for (auto& Arg : F->args()) {
    std::string name = Arg.getName().str();
    llvm::Type* arg_type = Arg.getType();
    
    llvm::AllocaInst* local_addr = builder.CreateAlloca(arg_type, 0, name);
    builder.CreateStore(&Arg, local_addr);
    this->visitorHelper.symtab_l[name] = local_addr;
      
    VLOG(6) << "Arg Symbol : " << name;
    print_llvm_type(arg_type);
    VLOG(6) << "";
  }

  VLOG(6) << "Finished Updating Symbol Table Argument";

  // 3. Call visitStatements()
  JackParser::StatementsContext* statements_ctx = ctx->statements();
  this->visitStatements(statements_ctx);

  VLOG(6) << "---- Finished Parsing SubroutineBody ----";

  return nullptr;
}


/* ------------------------ Visit Variables/Types ------------------------ */

antlrcpp::Any JackRealVisitor::visitVarDec(JackParser::VarDecContext *ctx) {
  // --------------------- //
  // Resolve variable type //
  // --------------------- //
  // Find out the type
  // then create LLVM::Type correspondingly
  JackParser::TypeContext* type_ctx = ctx->type();
  llvm::Type* varType = this->visitType(type_ctx).as<llvm::Type*>();

  // ----------------------- //
  // Construct return Vector //
  // ----------------------- //
  // vector = { {'static'/'field', nullptr}, {varname, type}, ... }
  std::vector<std::pair<std::string, llvm::Type*>> local_vars;

  // Find out class variable name
  std::vector<JackParser::VarNameContext*> var_name_ctxs = ctx->varName();
  for(const auto var_name_ctx : var_name_ctxs) {
    std::string var_name_str = this->visitVarName(var_name_ctx).as<std::string>();
    // Append {name, type} pairs to return vector
    local_vars.emplace_back(std::make_pair(var_name_str, varType));
  }

  return local_vars;
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
  llvm::Type* varType = this->visitType(type_ctx).as<llvm::Type*>();

  // ----------------------- //
  // Construct return Vector //
  // ----------------------- //
  // vector = { {'static'/'field', nullptr}, {varname, type}, ... }
  std::vector<std::pair<std::string, llvm::Type*>> class_vars = { {tok_text, nullptr} };

  // Find out class variable name
  std::vector<JackParser::VarNameContext*> var_name_ctxs = ctx->varName();
  for(const auto var_name_ctx : var_name_ctxs) {
    std::string var_name_str = this->visitVarName(var_name_ctx).as<std::string>();
    // Append {name, type} pairs to return vector
    class_vars.emplace_back(std::make_pair(var_name_str, varType));
  }

  return class_vars;
}

llvm::Type* JackRealVisitor::getVarType(antlr4::tree::TerminalNode* var_type, JackParser::ClassNameContext* class_type_ctx) {
  llvm::Type* varType;
  if(var_type) {
    antlr4::Token* var_type_tok = var_type -> getSymbol();
    std::string var_type_str = var_type_tok->getText();
    if(var_type_str == "int") {
      varType = llvm::Type::getInt32Ty(getContext());
    } else if(var_type_str == "char") {
      varType = llvm::Type::getInt8Ty(getContext());
    } else if(var_type_str == "boolean") {
      varType = llvm::Type::getInt1Ty(getContext());
    } else {
      assert(false && "Class variable basic types can only be 'int', 'char' or 'boolean'");
    }
    
    return varType;

  } else if(class_type_ctx) {
    // Get registered llvm::structType from llvm::Module
    std::string identifier_text = this->visitClassName(class_type_ctx).as<std::string>();
    
    varType = getModule().getTypeByName(identifier_text);
    assert(varType && "Class identifier used before declared");
    
    return varType;
  }

  return nullptr;
}

antlrcpp::Any JackRealVisitor::visitType(JackParser::TypeContext *ctx) {
  // --------------------------- //
  // Parse and return llvm::Type //
  // --------------------------- //
  antlr4::tree::TerminalNode* var_type = ctx->VARTYPES();
  JackParser::ArrayTypeContext* array_type_ctx = ctx->arrayType();
  JackParser::ClassNameContext* class_type_ctx = ctx->className();

  llvm::Type* varType;
  if(array_type_ctx) {
    varType = this->visitArrayType(array_type_ctx).as<llvm::Type*>();
  } else {
    varType = getVarType(var_type, class_type_ctx);
    assert(varType && "Class variable type has to be either 'var_type' or 'class_type'");
  }
  
  return varType;
}

antlrcpp::Any JackRealVisitor::visitArrayType(JackParser::ArrayTypeContext *ctx) {
  antlr4::tree::TerminalNode* var_type = ctx->VARTYPES();
  JackParser::ClassNameContext* class_type_ctx = ctx->className();
  
  antlr4::tree::TerminalNode* length_node = ctx->INTEGER();
  antlr4::Token* length_tok = length_node -> getSymbol();
  std::string length_string = length_tok->getText();
  size_t length = std::stoi(length_string);

  llvm::Type* varType = getVarType(var_type, class_type_ctx);
  assert(varType && "Unrecognized array type");
  
  llvm::Type* return_type = llvm::ArrayType::get(varType, length);

  return return_type;
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


antlrcpp::Any JackRealVisitor::visitSubroutineName(JackParser::SubroutineNameContext *ctx) {
  antlr4::tree::TerminalNode* subroutine_name = ctx->ID();
  antlr4::Token* subroutine_name_tok = subroutine_name -> getSymbol();
  std::string subroutine_name_str = subroutine_name_tok->getText();
  return subroutine_name_str;
  
}


llvm::Value* JackRealVisitor::variableLookup(std::string name, SYM_TYPE* type) {
  auto& builder = getBuilder();

  // lookup symtab_l
  if(this->visitorHelper.symtab_l.count(name)) {
    if(type) {
        *type = SYM_TYPE::LOCAL;
    }
    return this->visitorHelper.symtab_l[name];
  }
  
  // lookup self symtab_c
  if(this->visitorHelper.symtab_c.count(name)) {
    size_t index = this->visitorHelper.symtab_c[name];
    // Get 'this' and use GEP
    assert(this->visitorHelper.symtab_l.count("this") && "Member variable used outside of method/constructor subroutines");
    llvm::Value* this_addr = this->visitorHelper.symtab_l["this"];
    // Get value using GEP on this
    std::vector<llvm::Value*> indices(2);
    indices[0] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, 0, true)); // Get the pointer itself
    indices[1] = llvm::ConstantInt::get(getContext(), llvm::APInt(32, index, true));; // Get indexed member
    
    llvm::Value* member = builder.CreateGEP(this_addr, indices, "member");

    if(type) {
        *type = SYM_TYPE::CLASS_MEMBER;
    }
    return member;
  }
    
  // lookup Module->global
  // Get mangled global var name from class_var_func_name_mapping
  llvm::Type* this_type = getModule().getTypeByName(this->visitorHelper.current_class_name);
  std::string name_mangled = this->visitorHelper.class_globalvar_name_mapping[this_type][name];

  std::string error_message = "Undefined symbol used: " + name_mangled;
  assert(getModule().getGlobalVariable(name_mangled) && error_message.c_str());
  
  if(type) {
      *type = SYM_TYPE::GLOBAL;
  }
  return getModule().getGlobalVariable(name_mangled);
}
