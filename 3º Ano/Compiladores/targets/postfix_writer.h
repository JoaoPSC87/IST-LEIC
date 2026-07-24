#pragma once

#include "targets/basic_ast_visitor.h"
#include <set>
#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include <vector>

namespace p6 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<p6::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;


    bool _inFunction = false; // are we inside a function?
    int _offset = 0; // current offset in the function frame
    std::shared_ptr<p6::symbol> _function = nullptr; // current function (if any)
    bool _inFunctionArgs = false; // are we processing function arguments?
    std::string _currentBodyRetLabel; // label to jump to for function return
    std::set<std::string> _functions_to_declare; // functions that need to be declared in the RTS
    std::vector<int> _whileCond; // labels de início/condição (para next)
    std::vector<int> _whileEnd; // labels de fim (para stop)

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<p6::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // p6

