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
  COMMA ","
  QMARK "\""

  VAR "var"
  DEF "def"

;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%token <bool> BOOL_LITERAL "bool literal"

%token <std::string> TYPE_INT "type_int"
%token <std::string> TYPE_STRING "type_string"
%token <std::string> TYPE_BOOL "type_bool"

%type <StmtNode*> stmt funcDecl procDecl
%type <BlockNode*> stmts block

%type <StmtNode*> varDecl listaSpecsVar specVar
%type <StmtNode*> specVarSimples specVarSimplesIni
%type <StmtNode*> specVarArranjo specVarArranjoIni

%printer { yyoutput << $$; } <*>;

%start program;
%%

program: stmts { drv.program = $1; }
       ;

stmts: stmt { $$ = new BlockNode(); $$->stmts.push_back($1); }
      | stmts stmt { $1->stmts.push_back($2); $$ = $1; }
;

stmt: varDecl { $$ = $1; }
    | funcDecl { $$ = $1; }
    | procDecl { $$ = $1; }
    ;

varDecl: VAR listaSpecsVar COLON type SEMICOLON {};

listaSpecsVar: specVar {}
             | specVar COMMA listaSpecsVar {};

specVar: specVarSimples {}
       | specVarSimplesIni {}
       | specVarArranjo {}
       | specVarArranjoIni {};

specVarSimples: IDENTIFIER {};

specVarSimplesIni: IDENTIFIER ASSIGN QMARK stringChain QMARK {}
                 | IDENTIFIER ASSIGN NUMBER {};

stringChain: IDENTIFIER {}
	   | TYPE_INT {}
	   | TYPE_BOOL {}
	   | IDENTIFIER stringChain {}
	   | TYPE_INT stringChain {}
	   | TYPE_BOOL stringChain {}

specVarArranjo: IDENTIFIER LBRACKET NUMBER RBRACKET {};

specVarArranjoIni: {};

type: TYPE_INT {}
    | TYPE_STRING {}
    | TYPE_BOOL {};

funcDecl: DEF IDENTIFIER LPAREN RPAREN COLON type block { $$ = new FuncDeclNode($2, $6, $7); };
procDecl: DEF IDENTIFIER LPAREN RPAREN block { $$ = new FuncDeclNode($2, $5); };

block: LBRACE stmts RBRACE { $$ = $2; }
     | LBRACE RBRACE { $$ = new BlockNode(); };
%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
