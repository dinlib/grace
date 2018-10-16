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

%type <StmtNode*> stmt funcDecl procDecl
%type <BlockNode*> stmts block

%type <VarDeclNodeListStmt *> varDecl
%type <std::string> dataType
%type <SpecVar *> specVar specVarSimples specVarArranjo specVarSimplesIni specVarArranjoIni
%type <SpecVarList *> listaSpecsVar

%printer { yyoutput << $$; } <*>;

%start program;
%%

program: stmts                                                  { drv.program = $1; }
       ;

stmts: stmt                                                     { $$ = new BlockNode(); $$->stmts.push_back($1); }
      | stmts stmt                                              { $1->stmts.push_back($2); $$ = $1; }
      ;

stmt: varDecl                                                   { $$ = $1; }
    | funcDecl                                                  { $$ = $1; }
    | procDecl                                                  { $$ = $1; }
    ;

varDecl: VAR listaSpecsVar COLON dataType SEMICOLON             { $$ = new VarDeclNodeListStmt();
                                                                  for (auto spec : *$2)
                                                                    $$->varDeclList.push_back(new VarDeclNode(spec, $4));
                                                                }
       ;

listaSpecsVar: specVar                                          { $$ = new SpecVarList(); $$->push_back($1); }
             | specVar COMMA listaSpecsVar                      { $3->push_back($1); $$ = $3; }
             ;

specVar: specVarSimples                                         { $$ = $1; }
       | specVarSimplesIni                                      { $$ = $1; }
       | specVarArranjo                                         { $$ = $1; }
       | specVarArranjoIni                                      { $$ = $1; }
       ;

specVarSimples: IDENTIFIER                                      { $$ = new SpecVar($1, 0, NULL); }
              ;

specVarSimplesIni: IDENTIFIER ASSIGN STRING_LITERAL             { $$ = new SpecVar($1, 0, new StringAssignNode($1, $3)); }
                 | IDENTIFIER ASSIGN NUMBER                     { $$ = new SpecVar($1, 0, new NumberAssignNode($1, $3)); }
                 ;

specVarArranjo: IDENTIFIER LBRACKET NUMBER RBRACKET             { $$ = new SpecVar($1, $3, NULL); }
              ;

specVarArranjoIni: {};

dataType: TYPE_INT                                              { $$ = "type_int"; }
    | TYPE_STRING                                               { $$ = "type_string"; }
    | TYPE_BOOL                                                 { $$ = "type_bool"; };

funcDecl: DEF IDENTIFIER LPAREN RPAREN COLON dataType block     { $$ = new FuncDeclNode($2, $6, $7); };
procDecl: DEF IDENTIFIER LPAREN RPAREN block                    { $$ = new FuncDeclNode($2, $5); };

block: LBRACE stmts RBRACE                                      { $$ = $2; }
     | LBRACE RBRACE                                            { $$ = new BlockNode(); };
%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
