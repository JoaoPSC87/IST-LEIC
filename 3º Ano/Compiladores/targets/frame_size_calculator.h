#pragma once

#include "targets/basic_ast_visitor.h"
#include <sstream>

namespace p6 {

  /**
   * Percorre o corpo de uma função (ou do programa principal) e soma
   * os tamanhos de todas as variáveis locais, para reservar o espaço
   * do frame (instrução ENTER).
   */
  class frame_size_calculator: public basic_ast_visitor {
    cdk::symbol_table<p6::symbol> &_symtab;
    size_t _localsize;

  public:
    frame_size_calculator(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<p6::symbol> &symtab) :
        basic_ast_visitor(compiler), _symtab(symtab), _localsize(0) {
    }

  public:
    ~frame_size_calculator();

  public:
    size_t localsize() const {
      return _localsize;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // p6
