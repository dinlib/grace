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

  VAR "var"
  DEF "def"
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%token <bool> BOOL_LITERAL "bool literal"

%type <StmtNode*> stmt initVar varDecl funcDecl procDecl
%type <BlockNode*> stmts block

%printer { yyoutput << $$; } <*>;

%start program;
%%

program: stmts { drv.program = $1; }
       ;

stmts: stmt { $$ = new BlockNode(); $$->stmts.push_back($1); }
      | stmts stmt { $1->stmts.push_back($2); $$ = $1; }
      ;

stmt: varDecl { $$ = $1; }
    | initVar { $$ = $1; }
    | funcDecl { $$ = $1; }
    | procDecl { $$ = $1; }
    ;

initVar: VAR IDENTIFIER COLON IDENTIFIER SEMICOLON { $$ = new VarInitNode($2, $4); }

varDecl: VAR IDENTIFIER LBRACKET NUMBER RBRACKET COLON IDENTIFIER SEMICOLON { $$ = new VarDeclNode($2, $4, $7); };

funcDecl: DEF IDENTIFIER LPAREN RPAREN COLON IDENTIFIER block { $$ = new FuncDeclNode($2, $6, $7); };
procDecl:;

block: LBRACE stmts RBRACE { $$ = $2; }
     | LBRACE RBRACE { $$ = new BlockNode(); };

%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
