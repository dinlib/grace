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

  typedef std::pair<std::string, int> SpecVar
  typedef std::vector<SpecVar> SpecVarList
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
%type <std::string> dataType
%type <SpecVarList*> listaSpecsVar

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

varDecl: VAR listaSpecsVar COLON dataType SEMICOLON             { $$ = new BlockNode();
                                                                  for (auto &spec : *$2)
                                                                    $$->push_back(new VarDeclNode(spec.first, spec.second, $4));
                                                                  delete $2; 
                                                                };

listaSpecsVar: specVar                                          { $$ = new SpecVarList(); $$->push_back($1); }
             | specVar COMMA listaSpecsVar                      { $3->push_back($1); $$ = $1; };

specVar: specVarSimples                                         { $$ = $1; }
       | specVarSimplesIni                                      { $$ = $1; }
       | specVarArranjo                                         { $$ = $1; }
       | specVarArranjoIni                                      { $$ = $1; };

specVarSimples: IDENTIFIER                                      { $$ = make_pair($1, 0); };

specVarSimplesIni: IDENTIFIER ASSIGN QMARK stringChain QMARK    {}
                 | IDENTIFIER ASSIGN NUMBER {};

stringChain: IDENTIFIER {}
	   | TYPE_INT {}
	   | TYPE_BOOL {}
	   | IDENTIFIER stringChain {}
	   | TYPE_INT stringChain {}
	   | TYPE_BOOL stringChain {};

specVarArranjo: IDENTIFIER LBRACKET NUMBER RBRACKET             { $$ = make_pair($1, $3); };

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
