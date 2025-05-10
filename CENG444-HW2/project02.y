%language "c++"
%define api.parser.class {MyParserBase}

%{
using namespace std;
#include <iostream>
#include <fstream>
%}
%code requires
{
class MyParser;
};

%code 
{
   #include "MyParser.h"
   #define yylex(x) driver->lex(x)
   
   extern MyParser *parser;
}

%parse-param {MyParser *driver} 
%start statements

%token SOB
%token EOB
%token OB
%token CB
%token ID
%token STR
%token NUM
%token COLON
%token COMMA
%token BOOL
%token AND
%token OR
%token QM
%token EM
%token OP
%token CP
%token SEMICOLON
%token PLUS 
%token MINUS
%token MULT 
%token DIV
%token EQ 
%token NEQ
%token LT 
%token GT 
%token LTE 
%token GTE
%token ASSIGN
%token WAIT 
%token THEN
%% 


functionCall
   : OP expressionList CP                             {parser->functionCall_detected();}
   ;

arrayAccessList
   : OB expressionList CB                             {parser->arrayAccess_detected();}
   | OB expressionList CB arrayAccessList             {parser->arrayAccess_detected();}
   ;

arrayAccess
   : arrayAccessList
   ;

array 
   :  OB expressionList CB                               {parser->array_detected();}                     
   |  OB CB                                              {parser->array_detected();}
   ;

const
   : NUM                                                 {parser->num_detected();}
   | STR                                                 {parser->str_detected();}
   | BOOL                     
   | array                                               
   | OP expression  CP 
   | ID 
   ;

expressionEigth
   : functionCall
   | arrayAccess
   | functionCall arrayAccess
   |
   ;

expressionSeventh                                      
   : const expressionEigth
   ;

expressionSixth
   : MINUS expressionSeventh
   | EM expressionSeventh
   | expressionSeventh
   ;

expressionFifth
   : expressionSixth MULT expressionFifth
   | expressionSixth DIV expressionFifth
   | expressionSixth
   ;

expressionFourth
   : expressionFifth PLUS expressionFourth
   | expressionFifth MINUS expressionFourth
   | expressionFifth
   ;

expressionThird
   : expressionFourth EQ expressionThird
   | expressionFourth NEQ expressionThird
   | expressionFourth GT expressionThird
   | expressionFourth LT expressionThird
   | expressionFourth LTE expressionThird
   | expressionFourth GTE expressionThird
   | expressionFourth
   ;

expressionSecond
   : expressionThird  AND expressionSecond
   | expressionThird OR expressionSecond
   | expressionThird
   ;

expressionFirst
   : expressionSecond QM expressionFirst COLON expressionFirst    {parser->conditional_detected();}
   | expressionSecond
   ;

expression
   : expression ASSIGN expressionFirst                             {parser->assignment_detected();}
   | expressionFirst
   ;

expressionList
   : expression                              
   | expression COMMA expressionList
   ;

idList
   : ID
   | ID COMMA idList
   ;

waitExpr
   : WAIT idList  THEN expressionList
   | WAIT idList  THEN 
   ;

statementList
   : expressionList SEMICOLON                                      {parser->evaluation_detected();}
   | waitExpr SEMICOLON                                             
   | expressionList SEMICOLON statementList                        {parser->evaluation_detected();}
   | waitExpr SEMICOLON statementList
   ;

statements
   :  statementList 
   ;

%%
