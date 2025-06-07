
#ifndef MYPARSER_H_
#define MYPARSER_H_

#include "dgevalsup.h"

namespace yy
{
   class MyParserBase;
};

class MyFlexLexer;

class MyParser
{
   yy::MyParserBase *base=nullptr;
   MyFlexLexer      *lexer=nullptr;
   ofstream         *os=nullptr;

   // Variable to integrate token semantics
   yy::MyParserBase::semantic_type *lval=nullptr;

   int              parseErrorLine=-1;
   int              curLine = 1;          // ALKIM EKLEDİ

   string *makeString(const char *rawStr);
   int hexDigit(char c);
public:
   DGEval  *dgEval=nullptr;

   MyParser(ofstream *os);
   ~MyParser();

   void parse(yy::MyParserBase *base, ifstream *is);
   int lex(yy::MyParserBase::value_type *lval);

   int getId();
   int getStr();
   int getNumber();

   void setParseErrorLine();
   int getParseErrorLine();
   void incrementLine(); // ALKIM EKLEDİ

   // Parser actions
   // DGEvalExpNode *processExpStat(DGEvalExpNode *expression); // This is unused
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

   DGEvalStatementList *createStatementList(DGEvalStatementNode *statement);
   DGEvalStatementList *appendToStatementList(DGEvalStatementNode *statement);

   DGEvalExpStatement *createExpStatement(DGEvalExpNode *expression);
   DGEvalWaitStatement *createWaitStatement(DGEvalExpNode *expression, DGEvalIdentifierList *identifierList);

   DGEval *finalizeStatementList(DGEvalStatementList *statementList);

};


#endif /* MYPARSER_H_ */
