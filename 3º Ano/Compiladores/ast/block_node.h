#pragma once

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>

namespace p6 {

    class block_node : public cdk::basic_node {
        cdk::sequence_node *_declarations;
        cdk::sequence_node *_statements;
    
    public: 
        block_node(int lineno, cdk::sequence_node *declarations, cdk::sequence_node *statements) :
            cdk::basic_node(lineno), _declarations(declarations), _statements(statements) {}
            
        cdk::sequence_node *declarations() { return _declarations; }
        
        cdk::sequence_node *statements() { return _statements; }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_block_node(this, level);
        }
    };
}