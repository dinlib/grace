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
%token <std::string> STRING_LITERAL

%type <StmtNode*> stmt func_decl proc_decl
%type <BlockNode*> stmts block

%type <VarDeclNodeListStmt *> var_decl
%type <std::string> data_type
%type <SpecVar *> spec_var spec_var_simple spec_var_array spec_var_simple_init spec_var_array_init
%type <SpecVarList *> spec_var_list

%printer { yyoutput << $$; } <*>;

%start program;
%%

program: stmts                                                  { drv.program = $1; }
       ;

stmts: stmt                                                     { $$ = new BlockNode(); $$->stmts.push_back($1); }
      | stmts stmt                                              { $1->stmts.push_back($2); $$ = $1; }
      ;

stmt: var_decl                                                   { $$ = $1; }
    | func_decl                                                  { $$ = $1; }
    | proc_decl                                                  { $$ = $1; }
    ;

var_decl: VAR spec_var_list COLON data_type SEMICOLON             { $$ = new VarDeclNodeListStmt();
                                                                    for (auto spec : *$2) {
                                                                      $$->varDeclList.push_back(
                                                                        new VarDeclNode(spec->id, spec->size, spec->assign, $4)
                                                                      );
                                                                      delete spec;
                                                                    }
                                                                    delete $2;
                                                                  }
       ;

spec_var_list: spec_var                                          { $$ = new SpecVarList(); $$->push_back($1); }
             | spec_var COMMA spec_var_list                      { $3->push_back($1); $$ = $3; }
             ;

spec_var: spec_var_simple                                         { $$ = $1; }
       | spec_var_simple_init                                      { $$ = $1; }
       | spec_var_array                                         { $$ = $1; }
       | spec_var_array_init                                      { $$ = $1; }
       ;

spec_var_simple: IDENTIFIER                                      { $$ = new SpecVar($1, 0, NULL); }
              ;

spec_var_simple_init: IDENTIFIER ASSIGN STRING_LITERAL             { $$ = new SpecVar($1, 0, new StringAssignNode($1, $3)); }
                 | IDENTIFIER ASSIGN NUMBER                     { $$ = new SpecVar($1, 0, new NumberAssignNode($1, $3)); }
                 ;

spec_var_array: IDENTIFIER LBRACKET NUMBER RBRACKET             { $$ = new SpecVar($1, $3, NULL); }
              ;

spec_var_array_init:                                              {};

data_type: TYPE_INT                                              { $$ = "type_int"; }
    | TYPE_STRING                                               { $$ = "type_string"; }
    | TYPE_BOOL                                                 { $$ = "type_bool"; }
    ;

func_decl: DEF IDENTIFIER LPAREN RPAREN COLON data_type block     { $$ = new FuncDeclNode($2, $6, $7); };
proc_decl: DEF IDENTIFIER LPAREN RPAREN block                    { $$ = new FuncDeclNode($2, $5); };

block: LBRACE stmts RBRACE                                      { $$ = $2; }
     | LBRACE RBRACE                                            { $$ = new BlockNode(); };
%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
