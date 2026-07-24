#pragma once

#include <cdk/ast/expression_node.h>

namespace p6 {

    class sizeof_node: public cdk::expression_node {
        cdk::expression_node *_expr;
    public:
        sizeof_node(int lineno, cdk::expression_node *expr) :
            cdk::expression_node(lineno), _expr(expr) {
        }

        cdk::expression_node *expr() const {
            return _expr;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_sizeof_node(this, level);
        }
    };
}