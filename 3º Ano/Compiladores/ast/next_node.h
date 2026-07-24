#pragma once

#include <cdk/ast/basic_node.h>

namespace p6{

    class next_node: public cdk::basic_node {
        size_t _level;
    public:
        next_node(int lineno, size_t level) : cdk::basic_node(lineno), _level(level) {}
        
        size_t level() const {
            return _level;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_next_node(this, level);
        }
        

    }; 
}
