#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/types/functional_type.h>
#include <vector>
#include <memory>
#include <string>
#include "ast/block_node.h"

namespace p6{
    class function_node : public cdk::typed_node {
        int _qualifier;
        const std::string _name;
        cdk::sequence_node *_parameters;
        p6::block_node *_body;
    
    public:
        function_node(int lineno, int qualifier, const std::string &name, cdk::sequence_node *parameters, std::shared_ptr<cdk::basic_type> return_type, p6::block_node *body) :
            cdk::typed_node(lineno), _qualifier(qualifier), _name(name), _parameters(parameters), _body(body) {
                std::vector<std::shared_ptr<cdk::basic_type>> inputs;
                for (size_t i = 0; i < parameters->size(); i++)
                    inputs.push_back(dynamic_cast<cdk::typed_node*>(parameters->node(i))->type());
                type(cdk::functional_type::create(inputs, return_type));
            }
        
        int qualifier() const { return _qualifier; }       
        
        const std::string& name() const { return _name; }
        
        cdk::sequence_node* parameters() const { return _parameters; }
        
        p6::block_node* body() const { return _body; }
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_node(this, level);
        }
    };
}