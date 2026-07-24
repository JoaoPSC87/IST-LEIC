#pragma once

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace p6 {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    int _qualifier; // qualifiers: public, forward, "private" (i.e., none)
    bool _initialized; // tem inicializador?
    int _offset = 0; // 0 = global != 0 offset no frame local
    

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier , bool initialized) :
        _type(type), _name(name), _qualifier(qualifier), _initialized(initialized) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }

    void set_type(std::shared_ptr<cdk::basic_type> t) {
      _type = t;
    }

    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }

    bool is_function() const {
      return _type != nullptr && _type->name() == cdk::TYPE_FUNCTIONAL;
    }

    const std::string &name() const {
      return _name;
    }

    int qualifier() const { return _qualifier; }

    bool initialized() const { return _initialized; }

    int offset() const { return _offset; }

    void set_offset(int offset) { _offset = offset; }

    bool global() const { return _offset == 0; }
  };
    inline std::shared_ptr<symbol> make_symbol(
        std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier, 
        bool initialized) {
      return std::make_shared<symbol>(type, name, qualifier, initialized);
    }
  

} // p6

