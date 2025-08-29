#ifndef MYPARSER_H_
#define MYPARSER_H_

#include "dgevalsup.h"
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>

namespace yy
{
   class MyParserBase;
};

class MyFlexLexer;

class MyParser
{
   yy::MyParserBase *base=nullptr;
   MyFlexLexer      *lexer=nullptr;

   // Variable to integrate token semantics
   yy::MyParserBase::semantic_type *lval=nullptr;

   int              parseErrorLine=-1;

   string *makeString(const char *rawStr);
   int hexDigit(char c);
public:
   int      optimization;
   DGEval  *dgEval=nullptr;

   MyParser(int pOptimization);
   ~MyParser();

   void compile(yy::MyParserBase *pBase, ifstream *is, DGEval *dgEval);
   int lex(yy::MyParserBase::value_type *lval);

   int getId();
   int getStr();
   int getNumber();

   void setParseErrorLine();
   int getParseErrorLine();

   // Parser actions
   DGEvalExpNode *processBILRNode(DGEvalExpNode *baseNode, int opCode, DGEvalExpNode *newOperand);
   DGEvalExpNode *processBIRLNode(DGEvalExpNode *baseNode, int opCode, DGEvalExpNode *newOperand);
   DGEvalExpNode *processAssignmentSide(DGEvalExpNode *conditionNode, DGEvalExpNode *alternatives);
   DGEvalExpNode *processBinaryOperand(int pOpCode, DGEvalExpNode *operand);
   DGEvalExpNode *createPostfixNode(int pOpCode, DGEvalExpNode *operand);
   DGEvalExpNode *processUnaryOperand(DGEvalExpNode *baseOperand, DGEvalExpNode *postfixOperand);

   DGEvalExpNode *processNumericConstant(double value);
   DGEvalExpNode *processBooleanConstant(bool value);
   DGEvalExpNode *processStringConstant(string *str);
   DGEvalExpNode *processArrayLiteral(DGEvalExpNode *expression);
   DGEvalExpNode *processID(string *id);

   DGEvalIdentifierList *createIdentifierList(string *id);
   DGEvalIdentifierList *appendToIdentifierList(DGEvalIdentifierList *identifierList, string *id);

   DGEvalStatementList *appendToStatementList(DGEvalStatementNode *statement);

   DGEvalExpStatement *createExpStatement(DGEvalExpNode *expression);
   DGEvalWaitStatement *createWaitStatement(DGEvalExpNode *expression, DGEvalIdentifierList *identifierList);
};

#include "x64codegen.h"

#endif /* MYPARSER_H_ */
