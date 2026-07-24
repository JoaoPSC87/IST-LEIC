#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <string>


namespace p6 {

    class variable_declaration_node : public cdk::typed_node {
        int _qualifier;
        const std::string *_name;
        cdk::expression_node *_initializer;
        
    public:
        variable_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> varType, const std::string *name, cdk::expression_node *initializer) :
            cdk::typed_node(lineno), _qualifier(qualifier), _name(name), _initializer(initializer) {
            type(varType);    
            }

        int qualifier() const {
            return _qualifier;
        }

        const std::string& name() const {
            return *_name;
        }

        cdk::expression_node* initializer() const {
            return _initializer;
        }

         void accept(basic_ast_visitor *sp, int level) {
            sp->do_variable_declaration_node(this, level);
        }
    };
}