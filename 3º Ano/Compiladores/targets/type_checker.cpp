#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>
#include <cdk/types/balanced3_type.h>
#include <cdk/types/takum3_type.h>
#include <cdk/types/reference_type.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/functional_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

static bool is_numeric(std::shared_ptr<cdk::basic_type> type) {
  return type->name() == cdk::TYPE_BALANCED3 || type->name() == cdk::TYPE_TAKUM3;
}

void p6::type_checker::processBinaryComparison(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if(!is_numeric(node->left()->type()))
    throw std::string("wrong type in left argument of comparison expression");
  node->right()->accept(this, lvl + 2);
  if(!is_numeric(node->right()->type()))
    throw std::string("wrong type in right argument of comparison expression");
  node->type(cdk::balanced3_type::create());
}

void p6::type_checker::processBinaryLogical(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("wrong type in left argument of logical expression");
  node->right()->accept(this, lvl + 2);
  if(!node->right()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("wrong type in right argument of logical expression");
  node->type(cdk::balanced3_type::create());
}

void p6::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  // sequências são verificadas elemento-a-elemento por quem as contém (print, chamadas...)
}

//---------------------------------------------------------------------------

void p6::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // nó nulo: sem tipo a verificar
}
void p6::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // nó de dados auxiliar: sem tipo a verificar
}
void p6::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}
void p6::type_checker::do_balanced3_node(cdk::balanced3_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::balanced3_type::create());
}
void p6::type_checker::do_posit3_node(cdk::posit3_node *const node, int lvl) {
  // o P6 não usa posit3 (os reais são takum3) — nó nunca surge
}
void p6::type_checker::do_takum3_node(cdk::takum3_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::takum3_type::create());
}
void p6::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if(!node->argument()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("wrong type in argument of 'not' expression");
  node->type(cdk::balanced3_type::create());
}
void p6::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processBinaryLogical(node, lvl);
}
void p6::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processBinaryLogical(node, lvl);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void p6::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void p6::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!is_numeric(node->argument()->type())) throw std::string("wrong type in argument of unary expression");

  node->type(node->argument()->type());
}

void p6::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void p6::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void p6::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!is_numeric(node->left()->type()))
    throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!is_numeric(node->right()->type()))
    throw std::string("wrong type in right argument of binary expression");

  if(node->left()->is_typed(cdk::TYPE_TAKUM3) || node->right()->is_typed(cdk::TYPE_TAKUM3))
    node->type(cdk::takum3_type::create());
  else
    node->type(cdk::balanced3_type::create());
}

void p6::type_checker::processAdditiveExpression(cdk::binary_operation_node *const node, int lvl, bool isSub){
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  bool lptr = node->left()->is_typed(cdk::TYPE_POINTER);
  bool rptr = node->right()->is_typed(cdk::TYPE_POINTER);
  bool lint = node->left()->is_typed(cdk::TYPE_BALANCED3);
  bool rint = node->right()->is_typed(cdk::TYPE_BALANCED3);

  if (lptr && rint) {                    // ptr + int  /  ptr - int  → ptr
    node->type(node->left()->type());
  } else if (!isSub && lint && rptr) {   // int + ptr → ptr
    node->type(node->right()->type());
  } else if (isSub && lptr && rptr) {    // ptr - ptr → int
    node->type(cdk::balanced3_type::create());
  } else {
    if (!is_numeric(node->left()->type()))
      throw std::string("wrong type in left argument of additive expression");
    if (!is_numeric(node->right()->type()))
      throw std::string("wrong type in right argument of additive expression");
    if (node->left()->is_typed(cdk::TYPE_TAKUM3) || node->right()->is_typed(cdk::TYPE_TAKUM3))
      node->type(cdk::takum3_type::create());
    else
      node->type(cdk::balanced3_type::create()); 
  }
}

void p6::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processAdditiveExpression(node, lvl, false);
}
void p6::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processAdditiveExpression(node, lvl, true);
}
void p6::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void p6::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void p6::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("wrong type in left argument of modulus expression");
  node->right()->accept(this, lvl + 2);
  if(!node->right()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("wrong type in right argument of modulus expression");
  node->type(cdk::balanced3_type::create());
}
void p6::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}
void p6::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}
void p6::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}
void p6::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}
void p6::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}
void p6::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryComparison(node, lvl);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<p6::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void p6::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void p6::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  // Analisa o lado esquerdo e o lado direito
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  // Verificações de compatibilidade de tipos bem parecidas com as da declaração
  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_INT))
      throw std::string("wrong type in right argument of integer assignment");
  } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_DOUBLE) && !node->rvalue()->is_typed(cdk::TYPE_INT))
      throw std::string("wrong type in right argument of double assignment");
  } else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_STRING))
      throw std::string("wrong type in right argument of string assignment");
  } else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_POINTER) &&
        !node->rvalue()->is_typed(cdk::TYPE_VOID)) // null_node has TYPE_VOID
      throw std::string("wrong type in right argument of pointer assignment");
  } else if (node->lvalue()->is_typed(cdk::TYPE_BALANCED3)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_BALANCED3))
      throw std::string("wrong type in right argument of balanced3 assignment");
  } else if (node->lvalue()->is_typed(cdk::TYPE_TAKUM3)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_TAKUM3) && !node->rvalue()->is_typed(cdk::TYPE_BALANCED3))
      throw std::string("wrong type in right argument of takum3 assignment");
  } else {
    if (!node->rvalue()->is_typed(node->lvalue()->type()->name()))
      throw std::string("wrong types in assignment");
  }

  if (auto a = dynamic_cast<p6::alloc_node*>(node->rvalue()))
    a->type(node->lvalue()->type());

  // O tipo de uma expressão de atribuição é na maioria das linguagens C-like o tipo do lvalue
  node->type(node->lvalue()->type());
}

//---------------------------------------------------------------------------

void p6::type_checker::do_program_node(p6::program_node *const node, int lvl) {
  // o corpo é verificado nó-a-nó via ASSERT_SAFE_EXPRESSIONS no postfix_writer
}

void p6::type_checker::do_evaluation_node(p6::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void p6::type_checker::do_print_node(p6::print_node *const node, int lvl) {
  for(size_t i = 0; i < node->arguments()->size(); i++) {
    auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
    child->accept(this, lvl + 2);
    if (child->is_typed(cdk::TYPE_POINTER))
      throw std::string("pointer expression cannot be printed");
  }
}

//---------------------------------------------------------------------------


void p6::type_checker::do_while_node(p6::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("expected integer condition in while statement");
}

//---------------------------------------------------------------------------

void p6::type_checker::do_if_node(p6::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("expected integer condition in if statement");
}

void p6::type_checker::do_if_else_node(p6::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("expected integer condition in if-else statement");
}

void p6::type_checker::do_block_node(p6::block_node * const node, int lvl) {
  if (node->declarations()) {
    node->declarations()->accept(this, lvl + 2);
  }
  if (node->statements()) {
    node->statements()->accept(this, lvl + 2);
  }
}
void p6::type_checker::do_variable_declaration_node(p6::variable_declaration_node * const node, int lvl) {
  
  if (node->type() != nullptr && _symtab.find_local(node->name()) != nullptr){
    _parent->set_new_symbol(_symtab.find_local(node->name()));
    return;
  }

  if (node->initializer() != nullptr) {
    node->initializer()->accept(this, lvl + 2);

    if (node->type() == nullptr){
      node->type(node->initializer()->type());
    } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (!node->initializer()->is_typed(cdk::TYPE_DOUBLE) && !node->initializer()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type for double variable initializer");
    } else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING))
        throw std::string("wrong type for string variable initializer");
    } else if (node->is_typed(cdk::TYPE_POINTER)) {
      if (!node->initializer()->is_typed(cdk::TYPE_POINTER) &&
          !node->initializer()->is_typed(cdk::TYPE_VOID)) // null_node has TYPE_VOID internally in this setup
        throw std::string("wrong type for pointer variable initializer");
    } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
      if (!node->initializer()->is_typed(cdk::TYPE_BALANCED3))
        throw std::string("wrong type for balanced3 variable initializer");
    } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
      if (!node->initializer()->is_typed(cdk::TYPE_TAKUM3) && !node->initializer()->is_typed(cdk::TYPE_BALANCED3))
        throw std::string("wrong type for takum3 variable initializer");
    } else {
      if (!node->initializer()->is_typed(node->type()->name())) 
        throw std::string("wrong type in initializer of variable declaration");
    }
    if (auto a = dynamic_cast<p6::alloc_node*>(node->initializer()))
      a->type(node->type());
  }
  // O tipo void só pode ser retorno de função ou ponteiro genérico ([void]),
  // nunca o tipo de uma variável/parâmetro
  if (node->type() != nullptr && node->is_typed(cdk::TYPE_VOID)) {
    throw std::string("cannot declare variable of type void: " + node->name());
  }
  const std::string &id = node->name();

  // Criar e inserir na tabela de símbolos
  auto symbol = std::make_shared<p6::symbol>(node->type(), id, node->qualifier(), node->initializer() != nullptr);

  _symtab.insert(id, symbol);
  _parent->set_new_symbol(symbol);
}

void p6::type_checker::do_function_node(p6::function_node * const node, int lvl) {

  auto function = std::make_shared<p6::symbol>(node->type(), node->name(), node->qualifier(), false);

  _symtab.insert(node->name(), function);
  _parent->set_new_symbol(function);

  _symtab.push();
  if (node->parameters()) node->parameters()->accept(this, lvl);
  p6::type_checker checker(_compiler, _symtab, function, this);
  node->body()->accept(&checker, lvl + 2);
  _symtab.pop();
}
void p6::type_checker::do_function_call_node(p6::function_call_node * const node, int lvl) { 
  ASSERT_UNSPEC;
  
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);

  if (symbol == nullptr) {
    throw std::string("undeclared function '" + id + "'");
  }
  if (!symbol->is_function()) {
    throw std::string("symbol '" + id + "' is not a function");
  }
  
  auto ftype = cdk::functional_type::cast(symbol->type());

  if (node->arguments()->size() != ftype->input()->length()) {
    throw std::string("wrong number of arguments in call to function '" + id + "'");
  }

  for (size_t i = 0; i < node->arguments()->size(); i++) {
    auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
    arg->accept(this, lvl + 2);          // ← type-check de cada argumento
    auto paramType = ftype->input(i);
    if (arg->type()->name() != paramType->name()) {
      if (paramType->name() == cdk::TYPE_TAKUM3 && arg->is_typed(cdk::TYPE_BALANCED3))
        continue;
      throw std::string("wrong type in argument of call to function '" + id + "'");
    }
  }
  
  node->type(ftype->output(0));  // only one output type (single value)
}
void p6::type_checker::do_stop_node(p6::stop_node * const node, int lvl) {
  // 'stop' não tem tipo; a validação do nível é feita no postfix_writer
 }
void p6::type_checker::do_next_node(p6::next_node * const node, int lvl) { 
   // 'next' não tem tipo; a validação do nível é feita no postfix_writer
}
void p6::type_checker::do_return_node(p6::return_node * const node, int lvl) {
  std::shared_ptr<cdk::basic_type> f_type = nullptr;
  if (_function != nullptr) {
    auto ftype = cdk::functional_type::cast(_function->type());
    f_type = ftype->output(0);
  }

  if (node->return_value() != nullptr) {
    node->return_value()->accept(this, lvl + 2);
    if (f_type != nullptr) {
      if (f_type->name() == cdk::TYPE_VOID)
        throw std::string("return value specified for void function");

      if (f_type->name() == cdk::TYPE_STRING) {
        if (!node->return_value()->is_typed(cdk::TYPE_STRING))
          throw std::string("wrong type for string return expression");
      } else if (f_type->name() == cdk::TYPE_POINTER) {
        if (!node->return_value()->is_typed(cdk::TYPE_POINTER))
          throw std::string("wrong type for pointer return expression");
      } else if (f_type->name() == cdk::TYPE_BALANCED3) {
        if (!node->return_value()->is_typed(cdk::TYPE_BALANCED3))
          throw std::string("wrong type for integer return expression");
      } else if (f_type->name() == cdk::TYPE_TAKUM3) {
        if (!node->return_value()->is_typed(cdk::TYPE_TAKUM3) && !node->return_value()->is_typed(cdk::TYPE_BALANCED3))
          throw std::string("wrong type for real return expression");
      }
    }
  } else {
    if (f_type != nullptr && f_type->name() != cdk::TYPE_VOID)
      throw std::string("missing return value in non-void function");
  }
}
void p6::type_checker::do_input_node(p6::input_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::balanced3_type::create());
 }
void p6::type_checker::do_sizeof_node(p6::sizeof_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->expr()->accept(this, lvl + 2);
  node->type(cdk::balanced3_type::create());
 }
void p6::type_checker::do_address_of_node(p6::address_of_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
 }
void p6::type_checker::do_index_node(p6::index_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->pointer()->accept(this, lvl + 2);
  if (!node->pointer()->is_typed(cdk::TYPE_POINTER))
    throw std::string("pointer expression expected in index operator");
  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("integer expression expected in index operator");
  auto ref = cdk::reference_type::cast(node->pointer()->type());
  node->type(ref->referenced());
 }
void p6::type_checker::do_alloc_node(p6::alloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("integer expression expected in allocation expression");
  node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_VOID)));
 }
void p6::type_checker::do_null_node(p6::null_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_VOID)));
}
void p6::type_checker::do_function_declaration_node(p6::function_declaration_node * const node, int lvl) { 
  const std::string &id = node->name();
  auto symbol = _symtab.find_local(id);
  if (symbol == nullptr) {
    symbol = std::make_shared<p6::symbol>(node->type(), id, node->qualifier(), false);
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);
  }
}