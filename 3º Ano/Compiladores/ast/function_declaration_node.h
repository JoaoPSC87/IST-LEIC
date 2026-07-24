#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/basic_type.h>
#include <memory>
#include <string>

namespace p6{

    class function_declaration_node : public cdk::typed_node{
        int _qualifier;
        std::string _name;

    public:
        function_declaration_node(int lineno, int qualifier, const std::string &name, std::shared_ptr<cdk::basic_type> funcType) :
            cdk::typed_node(lineno), _qualifier(qualifier), _name(name) {
                type(funcType);
            }
            
        int qualifier() const{
            return _qualifier;
        }

        const std::string &name() const{
            return _name;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_declaration_node(this, level);
        }
        
    };

}