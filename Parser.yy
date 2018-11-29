%skeleton "lalr1.cc"
%require "3.0.4"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include "AST.hh"
  #include "Operators.hh"
  class Driver;
}

// The parsing context.
%param { Driver &drv }

%locations

%define parse.trace
%define parse.error verbose

%code { 
  #include "Driver.hh"
}

%define api.token.prefix {TOK_}
%token
  END 0 "end of file"
  ASSIGN "="
  MINUS "-"
  PLUS "+"
  STAR "*"
  SLASH "/"
  MOD "%"
  OR "||"
  AND "&&"
  NOT "!"
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
  DIFF "!="

  VAR "var"
  DEF "def"
  TRUE "true"
  FALSE "false"
  IF "if"
  ELSE "else"
  WHILE "while"
  FOR "for"
  RETURN "return"
  STOP "stop"
  SKIP "skip"
;

//definir precedência
%left OR
%left AND
%left EQ DIFF
%left LT LTEQ
%left GT GTEQ
%left PLUS MINUS
%left STAR SLASH MOD
%right NOT


%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%token <bool> BOOL_LITERAL "bool literal"

%token <std::string> TYPE_INT "type_int"
%token <std::string> TYPE_STRING "type_string"
%token <std::string> TYPE_BOOL "type_bool"
%token <std::string> STRING_LITERAL

%type <StmtNode*> stmt func_decl proc_decl if_then_else_stmt while_stmt for_stmt return_stmt assign_stmt
%type <BlockNode*> stmts Body

%type <VarDeclNodeListStmt *> var_decl
%type <std::string> data_type
%type <SpecVar *> spec_var spec_var_simple spec_var_array spec_var_simple_init spec_var_array_init
%type <SpecVarList *> spec_var_list
%type <ExprNode *> expr
%type <LiteralNode *> literal
%type <LiteralNodeList *> literal_list


%printer { yyoutput << $$; } <*>;

%start program;
%%

program: stmts { drv.program = $1; }
       ;

stmts: stmt { $$ = new BlockNode(); $$->stmts.push_back($1); }
      | stmts stmt { $1->stmts.push_back($2); $$ = $1; }
      ;

stmt: var_decl { $$ = $1; }
    | func_decl { $$ = $1; }
    | proc_decl { $$ = $1; }
	| if_then_else_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | for_stmt {$$ = $1; }
    | return_stmt { $$ = $1; }
    | SKIP SEMICOLON { $$ = new SkipNode(); }
    | STOP SEMICOLON { $$ = new StopNode(); }
    | assign_stmt { $$ = $1; }
    ;

if_then_else_stmt: IF LPAREN expr RPAREN Body { $$ = new IfThenElseNode($3, $5, NULL); }
				| IF LPAREN expr RPAREN Body ELSE Body { $$ = new IfThenElseNode($3, $5, $7); }
        ;

var_decl: VAR spec_var_list COLON data_type SEMICOLON { $$ = new VarDeclNodeListStmt();
                                                        for (auto spec : *$2) {
                                                          $$->varDeclList.push_back(
                                                            new VarDeclNode(spec->id, spec->size, spec->assign, $4)
                                                          );
                                                          delete spec;
                                                        }
                                                        delete $2;
                                                      }
       ;

spec_var_list: spec_var { $$ = new SpecVarList(); $$->push_back($1); }
             | spec_var_list COMMA spec_var { $1->push_back($3); $$ = $1; }
             ;

spec_var: spec_var_simple { $$ = $1; }
       | spec_var_simple_init { $$ = $1; }
       | spec_var_array { $$ = $1; }
       | spec_var_array_init { $$ = $1; }
       ;

spec_var_simple: IDENTIFIER { $$ = new SpecVar($1, 0, NULL); }
              ;

spec_var_simple_init: IDENTIFIER ASSIGN expr { $$ = new SpecVar($1, 0, new AssignSimpleNode($1, $3)); }
                    ;

spec_var_array: IDENTIFIER LBRACKET NUMBER RBRACKET { $$ = new SpecVar($1, $3, NULL); }
              ;

spec_var_array_init: IDENTIFIER LBRACKET NUMBER RBRACKET ASSIGN LBRACE literal_list RBRACE                                               { $$ = new SpecVar($1, $3, new ArrayAssignNode($1, $7)); };

literal_list: literal { $$ = new LiteralNodeList(); $$->push_back($1); } 
            | literal_list COMMA literal { $1->push_back($3); $$ = $1; }
            ;

literal: STRING_LITERAL { $$ = new LiteralStringNode($1); }
       | NUMBER { $$ = new LiteralIntNode($1); }
       | BOOL_LITERAL { $$ = new LiteralBoolNode($1); };

data_type: TYPE_INT { $$ = "type_int"; }
    | TYPE_STRING { $$ = "type_string"; }
    | TYPE_BOOL { $$ = "type_bool"; }
    ;

func_decl: DEF IDENTIFIER LPAREN RPAREN COLON data_type Body { $$ = new FuncDeclNode($2, $6, $7); };
proc_decl: DEF IDENTIFIER LPAREN RPAREN Body { $$ = new FuncDeclNode($2, $5); };

Body: LBRACE stmts RBRACE { $$ = $2; }
     | LBRACE RBRACE { $$ = new BlockNode(); };

expr: IDENTIFIER { $$ = new ExprIdentifierNode($1); }
    | literal { $$ = $1; }
    | MINUS expr { $$ = new ExprNegativeNode($2); }
    | expr PLUS expr { $$ = new ExprOperationNode($1, BinOp::PLUS, $3); }
    | expr MINUS expr { $$ = new ExprOperationNode($1, BinOp::MINUS, $3); }
    | expr STAR expr { $$ = new ExprOperationNode($1, BinOp::TIMES, $3); }
    | expr SLASH expr { $$ = new ExprOperationNode($1, BinOp::DIV, $3); }
    | expr MOD expr { $$ = new ExprOperationNode($1, BinOp::MOD, $3); }
    | expr LT expr { $$ = new ExprOperationNode($1, BinOp::LT, $3); }
    | expr LTEQ expr { $$ = new ExprOperationNode($1, BinOp::LTEQ, $3); }
    | expr GT expr { $$ = new ExprOperationNode($1, BinOp::GT, $3); }
    | expr GTEQ expr { $$ = new ExprOperationNode($1, BinOp::GTEQ, $3); }
    | expr EQ expr { $$ = new ExprOperationNode($1, BinOp::EQ, $3); }
    | expr DIFF expr { $$ = new ExprOperationNode($1, BinOp::DIFF, $3); }
    | expr AND expr { $$ = new ExprOperationNode($1, BinOp::AND, $3); }
    | expr OR expr { $$ = new ExprOperationNode($1, BinOp::OR, $3); }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

while_stmt: WHILE LPAREN expr RPAREN Body { $$ = new WhileNode($3, $5); };

for_stmt: FOR LPAREN var_decl expr SEMICOLON expr RPAREN Body { $$ = new ForNode($3, $4, $6, $8); };

return_stmt: RETURN SEMICOLON { $$ = new ReturnNode(NULL); }
            | RETURN expr SEMICOLON { $$ = new ReturnNode($2); };

assign_stmt: IDENTIFIER ASSIGN expr SEMICOLON { $$ = new AssignSimpleNode($1, $3); }
            | IDENTIFIER PLUS ASSIGN expr SEMICOLON { $$ = new CompoundAssignNode($1, BinOp::PLUS, $4); }
            | IDENTIFIER MINUS ASSIGN expr SEMICOLON { $$ = new CompoundAssignNode($1, BinOp::MINUS, $4); }
            | IDENTIFIER STAR ASSIGN expr SEMICOLON { $$ = new CompoundAssignNode($1, BinOp::TIMES, $4); }
            | IDENTIFIER SLASH ASSIGN expr SEMICOLON { $$ = new CompoundAssignNode($1, BinOp::DIV, $4); }
            ;

%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
