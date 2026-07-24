#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated
#include "targets/frame_size_calculator.h"

#define QUAL_PRIVATE 0
#define QUAL_PUBLIC  1
#define QUAL_EXTERN  2
#define QUAL_FORWARD 3
//---------------------------------------------------------------------------

// comparação ternária: chamada à RTS. Args right to left (cdecl).
// balanced3 = 8 bytes, takum3 = 16 bytes. Resultado balanced3 em eax:edx.

static void emit_cmp(cdk::binary_operation_node *const node, const std::string &op, basic_ast_visitor *self, cdk::basic_postfix_emitter &pf, int lvl) {
  std::string prefix;
  int arg_size;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) || node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    prefix = "takum3_";
    arg_size = 16;
  } else {
    prefix = "balanced3_";
    arg_size = 8;
  }

  node->right()->accept(self, lvl + 2);
  node->left()->accept(self, lvl + 2);
  pf.CALL(prefix + op);
  pf.TRASH(arg_size * 2);
  pf.LDFVAL64I();
}

void p6::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // nó nulo: não gera código
}
void p6::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // nó de dados auxiliar: não gera código
}
void p6::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  // o P6 não tem literais 'double' (os reais são takum3) — nó nunca surge
}
void p6::postfix_writer::do_balanced3_node(cdk::balanced3_node * const node, int lvl) {
  _pf.BALANCED3(node->value());
}
void p6::postfix_writer::do_posit3_node(cdk::posit3_node * const node, int lvl) {
  // o P6 não tem literais 'posit3' (os reais são takum3) — nó nunca surge
}
void p6::postfix_writer::do_takum3_node(cdk::takum3_node * const node, int lvl) {
  _pf.TAKUM3(node->value());
}
void p6::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.CALL("balanced3_kleene_not");
  _pf.TRASH(8);
  _pf.LDFVAL64I();
}
void p6::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lblEnd = ++_lbl;
  node->left()->accept(this, lvl); // [left(8)]
  _pf.DUP64(); // [left, left]
  _pf.B2I(); // [left, left_int]
  _pf.INT(0);
  _pf.JLT(mklbl(lblEnd)); // se left < 0 (F) → short-circuit, fica [left]
  // left >= 0: min(left, right)

  node->right()->accept(this, lvl);  // [left, right]
  _pf.CALL("balanced3_kleene_and");
  _pf.TRASH(16);
  _pf.LDFVAL64I(); // [resultado]
  _pf.LABEL(mklbl(lblEnd)); // ambos os caminhos convergem com 8 bytes
}
void p6::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lblEnd = ++_lbl;
  node->left()->accept(this, lvl); // [left]
  _pf.DUP64(); // [left, left]
  _pf.B2I(); // [left, left_int]
  _pf.INT(0);
  _pf.JGT(mklbl(lblEnd)); // se left > 0 (T) → short-circuit, fica [left]

  node->right()->accept(this, lvl); // [left, right]
  _pf.CALL("balanced3_kleene_or");
  _pf.TRASH(16);
  _pf.LDFVAL64I();
  _pf.LABEL(mklbl(lblEnd));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  _pf.INT(node->value()); // push an integer
}

void p6::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  /* leave the address on the stack */
  _pf.TEXT(); // return to the TEXT segment
  _pf.ADDR(mklbl(lbl1)); // the string to be printed
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_unary_minus_node(cdk::unary_minus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if(node->is_typed(cdk::TYPE_TAKUM3)) _pf.TNEG();
  else _pf.BNEG(); // 2-complement
}

void p6::postfix_writer::do_unary_plus_node(cdk::unary_plus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_POINTER)) {
    auto ref = cdk::reference_type::cast(node->type());
    int elemsize = ref->referenced()->size();
    if (node->left()->is_typed(cdk::TYPE_POINTER)) {
      node->left()->accept(this, lvl);
      node->right()->accept(this, lvl);
      _pf.B2I(); _pf.INT(elemsize); _pf.MUL();
    } else {
      node->left()->accept(this, lvl);
      _pf.B2I(); _pf.INT(elemsize); _pf.MUL();
      node->right()->accept(this, lvl);
    }
    _pf.ADD();
    return;
  }
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->left()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->right()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  if (node->is_typed(cdk::TYPE_TAKUM3)) _pf.TADD();
  else _pf.BADD();
}
void p6::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // ptr - ptr → nº de elementos
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    auto ref = cdk::reference_type::cast(node->left()->type());
    int elemsize = ref->referenced()->size();
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.SUB();
    _pf.INT(elemsize);
    _pf.DIV();
    _pf.I2B();
    return;
  }
  // ptr - int → ptr
  if (node->is_typed(cdk::TYPE_POINTER)) {
    auto ref = cdk::reference_type::cast(node->type());
    int elemsize = ref->referenced()->size();
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.B2I(); _pf.INT(elemsize); _pf.MUL();
    _pf.SUB();
    return;
  }
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->left()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->right()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  if (node->is_typed(cdk::TYPE_TAKUM3)) _pf.TSUB();
  else _pf.BSUB();
}

void p6::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->left()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->right()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  if (node->is_typed(cdk::TYPE_TAKUM3)) _pf.TMUL();
  else _pf.BMUL();
}
void p6::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->left()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3) && node->right()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
  if (node->is_typed(cdk::TYPE_TAKUM3)) _pf.TDIV();
  else _pf.BDIV();
}
void p6::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int la = ++_lbl, lb = ++_lbl, lr = ++_lbl, ls = ++_lbl;

  _pf.INT(24); _pf.ALLOC();                              // bloco temporário T, SP=T

  // ---- flag = (a>0 ? +1 : -1)  ->  [T+16] ----
  node->left()->accept(this, lvl);                      // [a]
  _pf.DUP64();
  _pf.INT(0); _pf.I2B();                                // balanced3 zero;  lt(0,a) = (a>0)
  _pf.CALL("balanced3_lt"); _pf.TRASH(16); _pf.LDFVAL64I();
  _pf.B2I();
  _pf.SP(); _pf.INT(28); _pf.ADD(); _pf.STINT();        // flag -> [T+16]

  // ---- |a| -> [T] ----
  _pf.SP(); _pf.INT(24); _pf.ADD(); _pf.LDINT();        // carrega flag
  _pf.INT(0); _pf.JGT(mklbl(la));                       // a>0: já é |a|
  _pf.BNEG();                                           // a<=0: nega
  _pf.LABEL(mklbl(la));
  _pf.SP(); _pf.INT(8); _pf.ADD(); _pf.STBALANCED3();   // |a| -> [T]

  // ---- |b| -> [T+8] ----
  node->right()->accept(this, lvl);                     // [b]
  _pf.DUP64();
  _pf.INT(0); _pf.I2B();
  _pf.CALL("balanced3_lt"); _pf.TRASH(16); _pf.LDFVAL64I();
  _pf.B2I();
  _pf.INT(0); _pf.JGT(mklbl(lb));
  _pf.BNEG();
  _pf.LABEL(mklbl(lb));
  _pf.SP(); _pf.INT(16); _pf.ADD(); _pf.STBALANCED3();  // |b| -> [T+8]

  // ---- resto positivo: rp = |a| BMOD |b|;  se rp<0 então rp += |b| ----
  _pf.SP(); _pf.LDBALANCED3();                          // |a|
  _pf.SP(); _pf.INT(16); _pf.ADD(); _pf.LDBALANCED3();  // |b|
  _pf.BMOD();
  _pf.DUP64(); _pf.B2I(); _pf.INT(0); _pf.JGE(mklbl(lr)); // rp>=0 ?
  _pf.SP(); _pf.INT(16); _pf.ADD(); _pf.LDBALANCED3();  // |b|
  _pf.BADD();
  _pf.LABEL(mklbl(lr));                                 // rpos em [0,|b|)

  // ---- aplicar sinal do dividendo: resultado = (a>0)? rpos : -rpos ----
  _pf.SP(); _pf.INT(24); _pf.ADD(); _pf.LDINT();        // flag
  _pf.INT(0); _pf.JGT(mklbl(ls));
  _pf.BNEG();
  _pf.LABEL(mklbl(ls));

  // ---- mover resultado p/ topo da pilha original e libertar o temporário ----
  _pf.SP(); _pf.INT(24); _pf.ADD(); _pf.STBALANCED3();  // resultado -> [T+16]
  _pf.TRASH(16);                                        // descarta [T,T+16); resultado no topo
}

void p6::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "lt", this, _pf, lvl);
}
void p6::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "le", this, _pf, lvl);
}
void p6::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "ge", this, _pf, lvl);
}
void p6::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "gt", this, _pf, lvl);
}
void p6::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "ne", this, _pf, lvl);
}
void p6::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  emit_cmp(node, "eq", this, _pf, lvl);
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  auto symbol = _symtab.find(node->name());
  if (symbol->global()){
    _pf.ADDR(symbol->name());
  } else {
    _pf.LOCAL(symbol->offset());
  }
}

void p6::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_TAKUM3)) _pf.LDTAKUM3();
  else if (node->is_typed(cdk::TYPE_BALANCED3)) _pf.LDBALANCED3();
  else _pf.LDINT();
}

void p6::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  if (node->is_typed(cdk::TYPE_TAKUM3)) {
    if (node->rvalue()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
    _pf.DUP128(); // duplicate the value to be assigned (16 bytes) for the store and the return value
    node->lvalue()->accept(this, lvl); // determine the address
    _pf.STTAKUM3();
  } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.DUP64(); // duplicate the value to be assigned (8 bytes) for the store and the return value
    node->lvalue()->accept(this, lvl); // determine the address
    _pf.STBALANCED3();
  } else {
    _pf.DUP32(); // duplicate the value to be assigned (4 bytes) for the store and the return value
    node->lvalue()->accept(this, lvl); // determine the address
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_program_node(p6::program_node * const node, int lvl) {

  // generate the main function (RTS mandates that its name be "_main")
  _function = nullptr;
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");

  frame_size_calculator fsc(_compiler, _symtab);
  node->body()->accept(&fsc, lvl);
  _pf.ENTER(fsc.localsize());

  _currentBodyRetLabel = mklbl(++_lbl);
  _offset = 0;
  _inFunction = true;
  node->body()->accept(this, lvl);
  // end the main function
  _inFunction = false;
  
  _pf.INT(0);
  _pf.STFVAL32I();
  _pf.LABEL(_currentBodyRetLabel);
  _pf.LEAVE();
  _pf.RET();

  // these are just a few library function imports
  _pf.EXTERN("balanced3_read");
  _pf.EXTERN("balanced3_print");
  _pf.EXTERN("takum3_read");
  _pf.EXTERN("takum3_print");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
  _pf.EXTERN("balanced3_lt");
  _pf.EXTERN("balanced3_le");
  _pf.EXTERN("balanced3_ge");
  _pf.EXTERN("balanced3_gt");
  _pf.EXTERN("balanced3_eq");
  _pf.EXTERN("balanced3_ne");
  _pf.EXTERN("takum3_lt");
  _pf.EXTERN("takum3_le");
  _pf.EXTERN("takum3_ge");
  _pf.EXTERN("takum3_gt");
  _pf.EXTERN("takum3_eq");
  _pf.EXTERN("takum3_ne");
  _pf.EXTERN("balanced3_kleene_not");
  _pf.EXTERN("balanced3_kleene_and");
  _pf.EXTERN("balanced3_kleene_or");
  for (auto &s : _functions_to_declare) _pf.EXTERN(s);

}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_evaluation_node(p6::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.TRASH(8); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_TAKUM3)) {
    _pf.TRASH(16); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING) || node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
    // do nothing
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void p6::postfix_writer::do_print_node(p6::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));

    child->accept(this, lvl); // expression to print

    if (child->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.CALL("balanced3_print");
      _pf.TRASH(8); // trash int

    } else if (child->is_typed(cdk::TYPE_TAKUM3)) {
      _pf.CALL("takum3_print");
      _pf.TRASH(16); // trash 2 ints
    } else if (child->is_typed(cdk::TYPE_STRING)) {
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    } else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }
  if (node->newline()) {
    _pf.CALL("println"); // print a newline
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_while_node(p6::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  lbl1 = ++_lbl;
  lbl2 = ++_lbl;
  _whileCond.push_back(lbl1);
  _whileEnd.push_back(lbl2);
  _pf.LABEL(mklbl(lbl1));
  node->condition()->accept(this, lvl);
  _pf.B2I();
  _pf.INT(0);
  _pf.JLE(mklbl(lbl2));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
  _whileCond.pop_back();
  _whileEnd.pop_back();
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_if_node(p6::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.B2I();
  _pf.INT(0);
  _pf.JLE(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_if_else_node(p6::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.B2I(); // convert balanced3 condition to int
  _pf.INT(0); // compare with 0 (false)
  _pf.JLE(mklbl(lbl1 = ++_lbl)); // if condition is false, jump to else block
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

void p6::postfix_writer::do_block_node(p6::block_node * const node, int lvl) {
  if (node->declarations()) {
    node->declarations()->accept(this, lvl + 2);
  }
  if (node->statements()) {
    node->statements()->accept(this, lvl + 2);  
  }
 }
void p6::postfix_writer::do_variable_declaration_node(p6::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  auto id = node->name();
  int typesize = node->type()->size();

  int offset = 0;
  if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize; 
  } else if (_inFunction) {
    _offset -= typesize;
    offset = _offset;
  }
  
  auto symbol = new_symbol();
  if (symbol) { symbol->set_offset(offset); reset_new_symbol(); }
  if(_inFunctionArgs) { return; } // arguments are handled in the function node

  // extern/forward: não gera nada
  if (node->qualifier() == QUAL_EXTERN || node->qualifier() == QUAL_FORWARD) return;

  if (_inFunction) {
    // local com inicializador → tratamos na Fase B; por agora ignora init local
    if (node->initializer()) {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_TAKUM3)) {
        if (node->initializer()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
        _pf.LOCAL(offset); _pf.STTAKUM3();
      } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
        _pf.LOCAL(offset); _pf.STBALANCED3();
      } else {
        _pf.LOCAL(offset); _pf.STINT();
      }
    }
  } else {
  // global
      if (node->initializer() == nullptr) {
        _pf.BSS();
        _pf.ALIGN();
        if (node->qualifier() == QUAL_PUBLIC) _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      } else if (node->is_typed(cdk::TYPE_STRING)) {
        // string: gerar o conteúdo em rodata, e o global aponta para ele
        auto lit = dynamic_cast<cdk::string_node*>(node->initializer());
        int strlbl = ++_lbl;
        _pf.RODATA();
        _pf.ALIGN();
        _pf.LABEL(mklbl(strlbl));
        _pf.SSTRING(lit->value());
        _pf.DATA();
        _pf.ALIGN();
        if (node->qualifier() == QUAL_PUBLIC) _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        _pf.SADDR(mklbl(strlbl));
      } else {
        _pf.DATA();
        _pf.ALIGN();
        if (node->qualifier() == QUAL_PUBLIC) _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
      if (node->is_typed(cdk::TYPE_BALANCED3)) {
        auto lit = dynamic_cast<cdk::balanced3_node*>(node->initializer());
        if (lit != nullptr) {
          _pf.SBALANCED3(lit->value());
        } else {
          // pode ser -literal (unary_minus de um balanced3_node)
          auto neg = dynamic_cast<cdk::unary_minus_node*>(node->initializer());
          auto inner = neg ? dynamic_cast<cdk::balanced3_node*>(neg->argument()) : nullptr;
          if (inner != nullptr) {
            _pf.SBALANCED3(cdk::balanced3_type::value_type(-inner->value().to_int64()));
          } else {
            std::cerr << node->lineno() << ": global integer initializer must be a literal" << std::endl;
          }
        }
      } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
        auto lit = dynamic_cast<cdk::takum3_node*>(node->initializer());
        if (lit != nullptr) {
          _pf.STAKUM3(lit->value());
        } else {
          std::cerr << node->lineno() << ": global real initializer must be a literal" << std::endl;
        }
      } else {
        std::cerr << "global initializer type not yet supported" << std::endl;
      }
    }
  }
}
void p6::postfix_writer::do_function_node(p6::function_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _function = new_symbol();
  reset_new_symbol();
  _functions_to_declare.erase(_function->name()); // remove from functions to declare, as we are now defining it

  std::string oldRetLabel = _currentBodyRetLabel;
  _currentBodyRetLabel = mklbl(++_lbl);

  auto rettype = cdk::functional_type::cast(_function->type())->output(0);
  _offset = (rettype->name() == cdk::TYPE_TAKUM3) ? 12 : 8; // ebp+8 = ponteiro sret
  _symtab.push(); // new scope for function parameters

  if (node->parameters()->size() > 0) {
    _inFunctionArgs = true;
    node->parameters()->accept(this, lvl);
    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (node->qualifier() == QUAL_PUBLIC) _pf.GLOBAL(_function->name(), _pf.FUNC());
  _pf.LABEL(_function->name());

  frame_size_calculator fsc(_compiler, _symtab);
  node->body()->accept(&fsc, lvl);
  _pf.ENTER(fsc.localsize());

  _offset = 0;
  bool oldInFunction = _inFunction;
  _inFunction = true;
  node->body()->accept(this, lvl + 2);
  _inFunction = oldInFunction;

  _pf.LABEL(_currentBodyRetLabel);
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop(); // pop function parameters scope
  _currentBodyRetLabel = oldRetLabel;
  _function = nullptr;
 }

void p6::postfix_writer::do_function_call_node(p6::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto symbol = _symtab.find(node->name());
  auto ftype = cdk::functional_type::cast(symbol->type());
  auto returnType = ftype->output(0);
  bool sret = (returnType->name() == cdk::TYPE_TAKUM3);

  if (sret) {
    _pf.INT(16);
    _pf.ALLOC();
  }

  size_t arg_size = 0;
  for(int ax = node->arguments()->size() - 1; ax >= 0; ax--){
    auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax));
    arg->accept(this, lvl + 2);
    auto paramtype = ftype->input(ax);
    if (paramtype->name() == cdk::TYPE_TAKUM3 && arg->is_typed(cdk::TYPE_BALANCED3)){
      _pf.B2T();
    }
    arg_size += paramtype->size();
  }

  if (sret) {
    _pf.SP();
    _pf.INT(arg_size);
    _pf.ADD();
  }

  _pf.CALL(node->name());
  
  if (sret){
    _pf.TRASH(arg_size + 4);

  } else{

    if (arg_size != 0)   _pf.TRASH(arg_size);
    
    if (returnType->name() == cdk::TYPE_BALANCED3) {
    _pf.LDFVAL64I();
    } else if (returnType->name() != cdk::TYPE_VOID) {
      _pf.LDFVAL32I();
    }
  }
}

void p6::postfix_writer::do_stop_node(p6::stop_node * const node, int lvl) {
  size_t level = node->level();
  if (level == 0 || level > _whileEnd.size()) {
    std::cerr << node->lineno() << ": invalid stop level" << std::endl;
    return;
  }
  _pf.JMP(mklbl(_whileEnd[_whileEnd.size() - level]));
 }
void p6::postfix_writer::do_next_node(p6::next_node * const node, int lvl) { 
  size_t level = node->level();
  if (level == 0 || level > _whileCond.size()){
    std::cerr << node->lineno() << ": invalid next level" << std::endl;
    return;
  }
  _pf.JMP(mklbl(_whileCond[_whileCond.size() - level]));
}

void p6::postfix_writer::do_return_node(p6::return_node * const node, int lvl) { 
  ASSERT_SAFE_EXPRESSIONS;
  if (_function == nullptr) {
    // return outside of function
    if (node->return_value()){
      node->return_value()->accept(this, lvl + 2);
      _pf.B2I();
      _pf.STFVAL32I();
    }
    _pf.JMP(_currentBodyRetLabel);
    return;
  }
  auto ftype = cdk::functional_type::cast(_function->type());
  auto returnType = ftype->output(0);
  if (returnType->name() != cdk::TYPE_VOID && node->return_value() != nullptr) {
    node->return_value()->accept(this, lvl + 2);
    if (returnType->name() == cdk::TYPE_TAKUM3) {
      if (node->return_value()->is_typed(cdk::TYPE_BALANCED3)) _pf.B2T();
      _pf.LOCAL(8);    // endereço do ponteiro sret (ebp+8)
      _pf.LDINT();     // carrega o endereço de destino
      _pf.STTAKUM3();  // grava os 16B lá (ordem: valor, depois endereço — como no assignment)
    } else if (returnType->name() == cdk::TYPE_BALANCED3) {
      _pf.STFVAL64I();
    } else {
      _pf.STFVAL32I();
    }
  }
  _pf.JMP(_currentBodyRetLabel);
}
void p6::postfix_writer::do_input_node(p6::input_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.CALL("balanced3_read");
    _pf.LDFVAL64I();
  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
  }
}

void p6::postfix_writer::do_sizeof_node(p6::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.BALANCED3(cdk::balanced3_type::value_type((long long)node->expr()->type()->size()));
 }
void p6::postfix_writer::do_address_of_node(p6::address_of_node * const node, int lvl) { 
  ASSERT_SAFE_EXPRESSIONS;

  node->lvalue()->accept(this, lvl + 2); 
}
void p6::postfix_writer::do_index_node(p6::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->pointer()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.B2I();
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
 }
void p6::postfix_writer::do_alloc_node(p6::alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);
  _pf.B2I();
  auto ref = cdk::reference_type::cast(node->type());
  int elemSize = (ref && ref->referenced()) ? ref->referenced()->size() : 4; // default to 4 if we can't determine the element size
  _pf.INT(elemSize);
  _pf.MUL();
  _pf.ALLOC(); 
  _pf.SP();
}
void p6::postfix_writer::do_null_node(p6::null_node * const node, int lvl) { 
  ASSERT_SAFE_EXPRESSIONS;
  
  if(_inFunction){
    _pf.INT(0);
  } else{
    _pf.SINT(0);
  }
}
void p6::postfix_writer::do_function_declaration_node(p6::function_declaration_node * const node, int lvl) { 
  ASSERT_SAFE_EXPRESSIONS;
  
  if(_inFunction || _inFunctionArgs) {
    std::cerr << "ERROR: cannot declare function inside another function" << std::endl;
    return;
  }

  if(!new_symbol()) return; // symbol already exists, error reported in TypeChecker

  auto function = new_symbol(); 
  _functions_to_declare.insert(function->name());
  reset_new_symbol();
}