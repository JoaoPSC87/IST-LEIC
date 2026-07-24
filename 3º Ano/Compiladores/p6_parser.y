%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#include <cdk/types/reference_type.h>
#include <vector>
#include <cdk/types/functional_type.h>
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
#define QUAL_PRIVATE 0
#define QUAL_PUBLIC 1
#define QUAL_EXTERN 2
#define QUAL_FORWARD 3
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;          /* integer value */
  std::string          *s;          /* symbol name or string literal */
  cdk::basic_node      *node;       /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  cdk::balanced3_type::value_type *b;
  cdk::takum3_type::value_type    *t;
  std::vector<std::shared_ptr<cdk::basic_type>> *types_vec;
  p6::block_node *block;
};

%token <b> tINTEGER
%token <t> tREAL
%token <s> tIDENTIFIER tSTRING
%token tWHILE tIF tPRINT tBEGIN tEND tPRINTLN tINT
%token tREALTYPE tSTRINGTYPE tVOID
%token tEXTERN tFORWARD tPUBLIC tAUTO
%token tELIF tSTOP tNEXT tRETURN
%token tINPUT tNULL tSIZEOF tARROW

%nonassoc tIFX
%nonassoc tELSE tELIF

%right '='
%left tOR
%left tAND
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc '~'
%nonassoc tUNARY '?' 
%nonassoc '['
%nonassoc DECL_PREC

%start file

%type <node> stmt program variable elif_chain
%type <block> block
%type <node> decl inner_decl var_decl func_decl func_def
%type <sequence> stmts exprs decls params inner_decls
%type <expression> expr
%type <lvalue> lval
%type <type> type
%type <i> qualifier
%type <s> string
%type <types_vec> types


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : decls                      { compiler -> ast($1); }
     | decls program              { compiler -> ast(new cdk::sequence_node(LINE, $2, $1)); }
     ; 

program : tBEGIN inner_decls stmts tEND  { $$ = new p6::program_node(LINE, new p6::block_node(LINE, $2, $3)); }
        | tBEGIN inner_decls tEND        { $$ = new p6::program_node(LINE, new p6::block_node(LINE, $2, new cdk::sequence_node(LINE))); }
        ;

decls : /* vazio */     { $$ = new cdk::sequence_node(LINE); }
      | decls decl      { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

inner_decls : /* vazio */              { $$ = new cdk::sequence_node(LINE); }
            | inner_decls inner_decl   { $$ = new cdk::sequence_node(LINE, $2, $1); }
            ;

decl       : var_decl | func_decl | func_def ;

inner_decl : var_decl | func_decl ;

var_decl : qualifier type tIDENTIFIER ';'              { $$ = new p6::variable_declaration_node(LINE, $1, $2, $3, nullptr); }
         | qualifier type tIDENTIFIER '=' expr ';'     { $$ = new p6::variable_declaration_node(LINE, $1, $2, $3, $5); }
         | qualifier tAUTO tIDENTIFIER '=' expr ';'    { $$ = new p6::variable_declaration_node(LINE, $1, nullptr, $3, $5); }
         ;

func_decl : qualifier type '<' '>' tIDENTIFIER ';'         { std::vector<std::shared_ptr<cdk::basic_type>> inputs;
                                                            $$ = new p6::function_declaration_node(LINE, $1, *$5, cdk::functional_type::create(inputs,$2)); delete $5; }
          | qualifier type '<' types '>' tIDENTIFIER ';'   { $$ = new p6::function_declaration_node(LINE, $1, *$6, cdk::functional_type::create(*$4, $2)); delete $4; delete $6; }
          ;

func_def : qualifier tIDENTIFIER '(' ')' tARROW type block          { $$ = new p6::function_node(LINE, $1, *$2, new cdk::sequence_node(LINE), $6, $7); delete $2; }
         | qualifier tIDENTIFIER '(' params ')' tARROW type block   { $$ = new p6::function_node(LINE, $1, *$2, $4, $7, $8); delete $2; }
         ;

params : variable                       { $$ = new cdk::sequence_node(LINE, $1); }
       | params ',' variable            { $$ = new cdk::sequence_node(LINE, $3, $1); }
       ;

variable : type tIDENTIFIER             { $$ = new p6::variable_declaration_node(LINE, 0, $1, $2, nullptr); }
         ;

types : type                            { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1);}
      | types ',' type                  { 
                                          $$ = $1; $$->push_back($3); }
      ;                                     

block : '{' inner_decls stmts '}'   { $$ = new p6::block_node(LINE, $2, $3); }
      | '{' inner_decls '}'         { $$ = new p6::block_node(LINE, $2, new cdk::sequence_node(LINE)); }
      ;

type : tINT                             { $$ = cdk::balanced3_type::create(); }
     | tREALTYPE                        { $$ = cdk::takum3_type::create(); } 
     | tSTRINGTYPE                      { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }   
     | tVOID                            { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
     | '[' type ']'                     { $$ = cdk::reference_type::create(4, $2);} 
     ;

qualifier : /* vazio */  %prec DECL_PREC  { $$ = QUAL_PRIVATE; }
          | tPUBLIC                       { $$ = QUAL_PUBLIC; }
          | tEXTERN                       { $$ = QUAL_EXTERN; }
          | tFORWARD                      { $$ = QUAL_FORWARD; }
          ;
     
stmts : stmt       { $$ = new cdk::sequence_node(LINE, $1); }
      | stmts stmt { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

stmt : expr ';'                         { $$ = new p6::evaluation_node(LINE, $1); }
     | exprs tPRINT                     { $$ = new p6::print_node(LINE, $1, false); }
     | exprs tPRINTLN                   { $$ = new p6::print_node(LINE, $1, true); }
     | tWHILE '(' expr ')' stmt         { $$ = new p6::while_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt %prec tIFX { $$ = new p6::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt elif_chain { $$ = new p6::if_else_node(LINE, $3, $5, $6); }
     | block                            { $$ = $1; }
     | tSTOP ';'                        { $$ = new p6::stop_node(LINE, 1); }
     | tSTOP tINTEGER ';'               { $$ = new p6::stop_node(LINE, (size_t)$2->to_int64()); delete $2; }
     | tNEXT ';'                        { $$ = new p6::next_node(LINE, 1); }
     | tNEXT tINTEGER ';'               { $$ = new p6::next_node(LINE, (size_t)$2->to_int64()); delete $2; }
     | tRETURN ';'                      { $$ = new p6::return_node(LINE, nullptr); }
     | tRETURN expr ';'                 { $$ = new p6::return_node(LINE, $2); }
     ;

elif_chain : tELSE stmt                           { $$ = $2; }
           | tELIF '(' expr ')' stmt %prec tIFX   { $$ = new p6::if_node(LINE, $3, $5); }
           | tELIF '(' expr ')' stmt elif_chain   { $$ = new p6::if_else_node(LINE, $3, $5, $6); }
           ;

expr : tINTEGER              { $$ = new cdk::balanced3_node(LINE, *$1); delete $1; }
     | tREAL                 { $$ = new cdk::takum3_node(LINE, *$1); delete $1;}
     | string                { $$ = new cdk::string_node(LINE, $1); }
     | tNULL                 { $$ = new p6::null_node(LINE); }
     /* Expressoes Primarias */
     | '(' expr ')'              { $$ = $2; }
     | '[' expr ']' %prec tUNARY { $$ = new p6::alloc_node(LINE, $2); } // rever isto futuramente
     /* Expressoes Unarias */
     | '-' expr %prec tUNARY { $$ = new cdk::unary_minus_node(LINE, $2); }
     | '+' expr %prec tUNARY { $$ = new cdk::unary_plus_node(LINE, $2); }
     | lval '?'              { $$ = new p6::address_of_node(LINE, $1); }
     | '~' expr              { $$ = new cdk::not_node(LINE, $2); }
     | tSIZEOF '(' expr ')'  { $$ = new p6::sizeof_node(LINE, $3); }
     /* Expressoes Aritiméticas */ 
     | expr '+' expr         { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr         { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr         { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr         { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr         { $$ = new cdk::mod_node(LINE, $1, $3); }
     /* Expressoes Logicas */
     | expr '<' expr         { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr         { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr         { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr         { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr         { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr         { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tAND expr        { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr         { $$ = new cdk::or_node(LINE, $1, $3); }
     /* Expressoes de Atribuição */
     | lval                  { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr         { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | tINPUT                { $$ = new p6::input_node(LINE); }
     /* Expressões de função */
     | tIDENTIFIER '(' ')'   {
                              $$ = new p6::function_call_node(LINE, *$1, new cdk::sequence_node(LINE));
                              delete $1;
                              }
     | tIDENTIFIER '(' exprs ')'    {
                                     $$ = new p6::function_call_node(LINE, *$1, $3);
                                     delete $1;
                                    }
     ;


exprs : expr                             { $$ = new cdk::sequence_node(LINE, $1); }
      | exprs ',' expr                   { $$ = new cdk::sequence_node(LINE, $3, $1); }
      ;

string : tSTRING                         { $$ = $1; }
       | string tSTRING                  { $$ = new std::string(*$1 + *$2); delete $2; }
       ;

lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     | expr '[' expr ']'       { $$ = new p6::index_node(LINE, $1, $3); }
     ;

%%
