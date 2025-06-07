%language "c++"
%define api.parser.class {MyParserBase}
   
%{
using namespace std;
#include <iostream>
#include <fstream>
%}
%code requires
{
#include <vector>
#include "dgevalsup.h"
};

%code 
{
   #include "MyParser.h"
   #define yylex(x) driver->lex(x)
}

%define api.value.type union
%nterm <DGEvalExpNode *>expression
%nterm <DGEvalExpNode *>commaOperand
%nterm <DGEvalExpNode *>assignmentSide
%nterm <DGEvalExpNode *>ternaryComponent
%nterm <DGEvalExpNode *>term
%nterm <DGEvalExpNode *>booleanOperand
%nterm <DGEvalExpNode *>comparisonOperand
%nterm <DGEvalExpNode *>factor
%nterm <DGEvalExpNode *>factorOperand
%nterm <DGEvalExpNode *>unaryOperand
%nterm <DGEvalExpNode *>unaryOperandBase
%nterm <int>booleanOperator
%nterm <int>comparisonOperator
%nterm <int>termOperator
%nterm <int>factorOperator
%nterm <DGEvalExpNode *>postfixOperator
%nterm <DGEvalExpNode *>actualParameters
%nterm <int>unaryOperator
%nterm <DGEvalIdentifierList *>identifierList
%nterm <DGEvalStatementList *>statementList
%nterm <DGEvalWaitStatement *>waitStatement
%nterm <DGEvalExpStatement *>expStatement
%nterm <DGEvalStatementNode *>statement
%nterm <DGEval *>dgeval;

%parse-param {MyParser *driver} 
%start dgeval

%token OB
%token CB
%token PLUS
%token MINUS
%token MUL
%token DIV
%token LP
%token RP
%token ASSIGN
%token LT
%token LTE
%token GT
%token GTE
%token EQ
%token NEQ
%token BAND
%token BOR
%token <string *>ID
%token <string *>STR
%token <double> NUM
%token COMMA
%token NOT
%token SEMICOLON
%token COLON
%token QMARK
%token WAIT
%token THEN
%token TRUE
%token FALSE

%%
   
dgeval: statementList              {$$=driver->finalizeStatementList($1);};

statementList
		: statement                 {$$=driver->appendToStatementList($1);}
		| statementList statement   {$$=driver->appendToStatementList($2);};

statement
	  : expStatement                          {$$=$1;}
      | waitStatement                         {$$=$1;};
        

expStatement
		: expression SEMICOLON                  {$$=driver->createExpStatement($1);};
		
waitStatement
		: WAIT identifierList THEN expression SEMICOLON   {$$=driver->createWaitStatement($4, $2);};

expression
		: commaOperand                          {$$=$1;}
      | expression COMMA commaOperand         {$$=driver->processBILRNode($1, OP_COMMA, $3);};
		
commaOperand
	  : assignmentSide                        {$$=$1;}
      | commaOperand ASSIGN assignmentSide    {$$=driver->processBIRLNode($1, OP_ASSIGN, $3);};
		
assignmentSide
		: booleanOperand ternaryComponent       {$$=driver->processAssignmentSide($1, $2);};    
		
ternaryComponent
		: QMARK assignmentSide COLON assignmentSide      {$$=driver->processBILRNode($2, OP_ALT, $4);}
		| {$$=nullptr;};
		
booleanOperand
      : comparisonOperand                                {$$=$1;}
      | booleanOperand booleanOperator comparisonOperand {$$=driver->processBILRNode($1, $2, $3);};		
      
booleanOperator
      : BAND                                           {$$=OP_BAND;}
      | BOR                                            {$$=OP_BOR;};
      
comparisonOperand
      : term                                           {$$=$1;}
      | comparisonOperand comparisonOperator term      {$$=driver->processBILRNode($1, $2, $3);};
      
comparisonOperator
      : EQ                                             {$$=OP_EQ;}
      | NEQ                                            {$$=OP_NEQ;}
      | LT                                             {$$=OP_LT;}
      | LTE                                            {$$=OP_LTE;}
      | GT                                             {$$=OP_GT;}
      | GTE                                            {$$=OP_GTE;};
      
term  : factor                                         {$$=$1;}
      | term termOperator factor                       {$$=driver->processBILRNode($1, $2, $3);};
      
termOperator
      : PLUS                                           {$$=OP_ADD;}
      | MINUS                                          {$$=OP_SUB;};
      
factor
      : factorOperand                                  {$$=$1;}
      | factor factorOperator factorOperand            {$$=driver->processBILRNode($1, $2, $3);};
      
factorOperator
		: MUL                                            {$$=OP_MUL;}
		| DIV                                            {$$=OP_DIV;};
		
factorOperand
		: unaryOperator unaryOperand               {$$=driver->processBinaryOperand($1,$2);}
		| unaryOperand                             {$$=$1;};
		
unaryOperator
		: MINUS                                    {$$=OP_MINUS;}
		| NOT                                      {$$=OP_NOT;};
		
unaryOperand
	   : unaryOperandBase                         {$$=$1;}
	   | unaryOperand postfixOperator             {$$=driver->processUnaryOperand($1, $2);};
		
unaryOperandBase
        : NUM                                      {$$=driver->processNumericConstant($1);}
		| TRUE                                     {$$=driver->processBooleanConstant(true);}
		| FALSE                                    {$$=driver->processBooleanConstant(false);}
		| STR                                      {$$=driver->processStringConstant($1);}	
		| OB expression CB                         {$$=driver->processArrayLiteral($2);}
		| ID                                       {$$=driver->processID($1);}
		| LP expression RP                         {$$=$2;};
		
actualParameters:
		expression                                 {$$=$1;}
		|                                          {$$=nullptr;};
		
postfixOperator
	    : OB expression CB                        {$$=driver->createPostfixNode(OP_AA, $2);}
	    | LP actualParameters RP                  {$$=driver->createPostfixNode(OP_CALL, $2);};
	    	    
identifierList
		: ID                                       {$$=driver->createIdentifierList($1);}
		| identifierList COMMA ID                  {$$=driver->appendToIdentifierList($1, $3);};
		
%%