#pragma once

#include <cdk/types/balanced3_type.h>
#include <cdk/ast/sequence_node.h>
#include "ast/function_node.h"

namespace p6 {

  /**
   * Class for describing program nodes.
   */
  class program_node : public p6::function_node {


  public:
    program_node(int lineno, p6::block_node *body) :
        function_node(lineno, 0, "_main", new cdk::sequence_node(lineno), cdk::balanced3_type::create(), body) {}
    

    void accept(basic_ast_visitor *sp, int level) { sp->do_program_node(this, level); }

  };

} // p6

