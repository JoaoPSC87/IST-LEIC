%option c++ prefix="p6_scanner_" outfile="p6_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include <cdk/types/balanced3_type.h>
#include <cdk/types/takum3_type.h>
#include "p6_parser.tab.h"

// don't change this
#define yyerror LexerError
static constexpr long long MAX_40_TRITS =  6078832729528464400LL;
static constexpr long long MIN_40_TRITS = -6078832729528464400LL;
%}
%x X_STRING X_COMMENT X_STRSKIP
%%
  yydebug=0; set_debug(0);


  /* ====================================================================== */
  /* ====[                       Comentarios                          ]==== */
  /* ====================================================================== */

"//".*                ; /* ignore, single line comments */
"/*"                  yy_push_state(X_COMMENT);
<X_COMMENT>"/*"       yy_push_state(X_COMMENT);
<X_COMMENT>"*/"       yy_pop_state();
<X_COMMENT>\n         ; /* ainda no comentário */
<X_COMMENT>.          ; /* ignore */


  /* ====================================================================== */
  /* ====[                  Tipos de dados                            ]==== */
  /* ====[                  Constantes                                ]==== */
  /* ====[                  Símbolos globais                          ]==== */
  /* ====================================================================== */

"int"                  return tINT;
"real"                 return tREALTYPE;
"string"               return tSTRINGTYPE;
"void"                 return tVOID;
"extern"               return tEXTERN;
"forward"              return tFORWARD;
"public"               return tPUBLIC;
"auto"                 return tAUTO;


  /* ====================================================================== */
  /* ====[                 Instrução condicional                      ]==== */
  /* ====================================================================== */
  

"if"                   return tIF; 
"else"                 return tELSE; 
"elif"                 return tELIF;
"stop"                 return tSTOP;


  /* ====================================================================== */
  /* ====[                 Instruções de Repetição                    ]==== */
  /* ====================================================================== */

"while"                return tWHILE; 
"next"                 return tNEXT;

  /* ====================================================================== */
  /* ====[                 Instruções para Funções                    ]==== */
  /* ====================================================================== */

"begin"                return tBEGIN; 
"end"                  return tEND; 
"return"               return tRETURN;


  /* ====================================================================== */
  /* ====[                     Operadores de expressões               ]==== */
  /* ====[                     Expressões especiais                   ]==== */
  /* ====================================================================== */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"!"                    return tPRINT; 
"!!"                   return tPRINTLN;
"input"                return tINPUT;
"null"                 return tNULL;
"sizeof"               return tSIZEOF;
"&&"                   return tAND;
"||"                   return tOR;
"->"                   return tARROW; 


  /* ====================================================================== */
  /* ====[                 Identificadores (nomes)                    ]==== */
  /* ====================================================================== */

[A-Za-z][A-Za-z0-9]*  yylval.s = new std::string(yytext); return tIDENTIFIER;


  /* ====================================================================== */
  /* ====[               Cadeias de caracteres                        ]==== */
  /* ====================================================================== */

\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;

<X_STRING>\\t          *yylval.s += '\t';
<X_STRING>\\n          *yylval.s += '\n';
<X_STRING>\\r          *yylval.s += '\r';
<X_STRING>\\\\         *yylval.s += '\\';
<X_STRING>\\\"         *yylval.s += '"';
<X_STRING>\\[0-7]{1,3} {
                        unsigned long c = std::stoul(yytext + 1, nullptr, 8);
                        if (c > 255) yyerror("octal escape out of range");
                        if (c == 0) { yy_pop_state(); yy_push_state(X_STRSKIP); }
                        else *yylval.s += static_cast<char>(c);
                       }
<X_STRING>\n           yyerror("newline in string");
<X_STRING>.            *yylval.s += yytext;
<X_STRSKIP>\"          yy_pop_state(); return tSTRING;
<X_STRSKIP>\\.         ;
<X_STRSKIP>\n          yyerror("newline in string");
<X_STRSKIP>.           ;

  /* ====================================================================== */
  /* ====[                      Números Reais                         ]==== */
  /* ====================================================================== */

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([Ee]([-+])?[0-9]+)? { yylval.t = new cdk::takum3_type::value_type(yytext); return tREAL; }

[0-9]+[Ee][-+]?[0-9]+     { yylval.t = new cdk::takum3_type::value_type(yytext); return tREAL; }



  /* ====================================================================== */
  /* ====[                       Números Inteiros                     ]==== */
  /* ====================================================================== */

0t[+0\-]+             {
                        /* converter manualmente: + = +1, 0 = 0, - = -1 */
                        long long val = 0;
                        bool overflow = false;
                        for (int i = 2; i < yyleng; i++) {
                          if (val > MAX_40_TRITS / 3 || val < MIN_40_TRITS / 3) {
                            overflow = true; break;
                          }
                          val *= 3;
                          if (yytext[i] == '+') val += 1;
                          else if (yytext[i] == '-') val -= 1;
                        }
                        if (overflow) yyerror("Integer literal out of range");
                        yylval.b = new cdk::balanced3_type::value_type(val);
                        return tINTEGER;
                      }


0|[1-9][0-9]*         {
                       try{
                        long long v = std::stoll(yytext, NULL, 10);
                        if(v > MAX_40_TRITS) yyerror("Integer literal out of range");
                        yylval.b = new cdk::balanced3_type::value_type(v);
                        return tINTEGER;
                       } catch (const std::out_of_range& a) {
                        yyerror("Integer literal out of range");
                       }
                      }

0[0-8]+               {
                            try{
                              long long v = std::stoll(yytext, NULL, 9);
                              if (v > MAX_40_TRITS) yyerror("Integer literal out of range");
                              yylval.b = new cdk::balanced3_type::value_type(v);
                              return tINTEGER;
                            } catch (const std::out_of_range& a){
                              yyerror("Integer literal out of range");
                            }
                      }

0[0-9]+               yyerror("invalid base-9 number");

  /* ====================================================================== */
  /* ====[                        Resto                               ]==== */
  /* ====================================================================== */
  
[-,()<>=+*/%;{}.\[\]\?~]    return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%
