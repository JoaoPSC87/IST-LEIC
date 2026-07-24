#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <string>


namespace p6{
    
    class function_call_node: public cdk::expression_node {
        cdk::sequence_node *_arguments;
        const std::string _name;

    
    public:
        function_call_node(int lineno, const std::string &name, cdk::sequence_node *arguments) :
            cdk::expression_node(lineno), _arguments(arguments), _name(name) {}
        
        const std::string &name() const {
            return _name;
        }

        cdk::sequence_node *arguments() const {
            return _arguments;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_call_node(this, level);
        }
    };  
}