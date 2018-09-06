%skeleton "lalr1.cc"
%require "3.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include "ast.hh"
  class Driver;
}

// The parsing context.
%param { Driver &drv }

%locations

%define parse.trace
%define parse.error verbose

%code { 
  #include "driver.hh"
}

%define api.token.prefix {TOK_}
%token 
  END 0 "end of file"
  ASSIGN "="
  MINUS "-"
  PLUS "+"
  STAR "*"
  SLASH "/"
  LPAREN "("
  RPAREN ")"
  LBRACKET "["
  RBRACKET "]"
  LBRACE "{"
  RBRACE "}"
  COLON ":"
  SEMICOLON ";"
  EQ "=="
  LT "<"
  LTEQ "<="
  GT ">"
  GTEQ ">="

  VAR "var"
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%token <bool> BOOL_LITERAL "bool literal"

%type <VarDeclNode*> varDecl
%type <BlockNode*> varDeclList

%printer { yyoutput << $$; } <*>;

%start program;
%%
program: varDeclList END { drv.program = $1; };

varDeclList: varDecl { $$ = new BlockNode(); $$->nodes.push_back($1); }
           | varDeclList varDecl { $1->nodes.push_back($2); $$ = $1; };

varDecl: VAR IDENTIFIER LBRACKET NUMBER RBRACKET COLON IDENTIFIER SEMICOLON { $$ = new VarDeclNode($2, $4, $7); };

%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}