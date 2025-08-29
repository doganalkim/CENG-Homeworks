using namespace std;
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include "dgevalsup.h"

#include "dgevalsyn.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"
#include "MyParser.h"

// Parser actions
DGEvalExpNode *MyParser::processBILRNode(DGEvalExpNode *pLeft, int opCode, DGEvalExpNode *pRight)
{
   return new DGEvalExpNode(lexer->lineno(), pLeft, opCode, pRight);
}

DGEvalExpNode *MyParser::processBIRLNode(DGEvalExpNode *pLeft, int opCode, DGEvalExpNode *pRight)
{
   DGEvalExpNode *cursor=pLeft,
                 *parent=nullptr;

   while (cursor->right!=nullptr)
   {
      parent=cursor;
      cursor=cursor->right;
   }

   if (parent==nullptr)
      return new DGEvalExpNode(lexer->lineno(), pLeft, opCode, pRight);

   parent->right=new DGEvalExpNode(lexer->lineno(), cursor, opCode, pRight);

   return pLeft;
}

DGEvalExpNode *MyParser::processBinaryOperand(int pOpCode, DGEvalExpNode *operand)
{
   return new DGEvalExpNode(lexer->lineno(), operand, pOpCode);
}

DGEvalExpNode *MyParser::createPostfixNode(int pOpCode, DGEvalExpNode *operand)
{
   return new DGEvalExpNode(lexer->lineno(), nullptr, pOpCode, operand);
}

DGEvalExpNode *MyParser::processUnaryOperand(DGEvalExpNode *baseOperand, DGEvalExpNode *postfixOperand)
{
   postfixOperand->left=baseOperand;

   return postfixOperand;
}

DGEvalExpNode *MyParser::processAssignmentSide(DGEvalExpNode *conditionNode, DGEvalExpNode *alternatives)
{
   if (alternatives==nullptr)
      return conditionNode;

   return new DGEvalExpNode(lexer->lineno(), conditionNode, OP_COND, alternatives);
}

DGEvalExpNode *MyParser::processNumericConstant(double value)
{
   return new DGEvalExpNode(lexer->lineno(), value, CONST);
}

DGEvalExpNode *MyParser::processStringConstant(string *value)
{
   return new DGEvalExpNode(lexer->lineno(), value, CONST);
}

DGEvalExpNode *MyParser::processID(string *value)
{
   return new DGEvalExpNode(lexer->lineno(), value, INSID);
}

DGEvalExpNode *MyParser::processBooleanConstant(bool value)
{
   return new DGEvalExpNode(lexer->lineno(), value, CONST);
}

DGEvalExpNode *MyParser::processArrayLiteral(DGEvalExpNode *expression)
{
   DGEvalExpNode *node=new DGEvalExpNode(lexer->lineno(), expression, LRT);
   node->idNdx=0;

   return node;
}

DGEvalIdentifierList *MyParser::createIdentifierList(string *id)
{
   DGEvalIdentifierList *retVal=new DGEvalIdentifierList(id);

   return retVal;
}

DGEvalIdentifierList *MyParser::appendToIdentifierList(DGEvalIdentifierList *identifierList, string *id)
{
   identifierList->append(id);

   return identifierList;
}

DGEvalExpStatement *MyParser::createExpStatement(DGEvalExpNode *expression)
{
   return new DGEvalExpStatement(lexer->lineno(), expression);
}


DGEvalWaitStatement *MyParser::createWaitStatement(DGEvalExpNode *expression, DGEvalIdentifierList *identifierList)
{
   return new DGEvalWaitStatement(lexer->lineno(), expression, identifierList);
}

DGEvalStatementList *MyParser::appendToStatementList(DGEvalStatementNode *statement)
{
   dgEval->statements->append(statement);
   return dgEval->statements;
}

// DGEvalMsgSet methods
void DGEvalMsgSet::writeAsJSON(ostream *os)
{

   auto                 c=messages.begin();
   size_t               cnt=messages.size();

   (*os)<<"[";

   for (size_t i=0;i<cnt;i++, c++)
   {
      if (i>0)
         (*os)<<", ";

      if ((*c)->lineNumber==INT_MAX || (*c)->lineNumber==INT_MIN)
         (*os)<<"\"["+DGEval::severityStr[(int)(*c)->s]+"]: "+(*c)->msg+"\"";
      else
         (*os)<<"\"Line number "+to_string((*c)->lineNumber)+" ["+DGEval::severityStr[(int)(*c)->s]+"]: "+(*c)->msg+"\"";
   }
   (*os)<<"]";
}

void DGEvalMsgSet::writeToConsole()
{
   auto                 c=messages.begin();
   size_t               cnt=messages.size();

   for (size_t i=0;i<cnt;i++, c++)
   {
      if ((*c)->lineNumber==INT_MAX || (*c)->lineNumber==INT_MIN)
         cout<<"["+DGEval::severityStr[(int)(*c)->s]+"]: "+(*c)->msg;
      else
         cout<<"Line number "+to_string((*c)->lineNumber)+" ["+DGEval::severityStr[(int)(*c)->s]+"]: "+(*c)->msg;

      cout << endl;
   }
}

// DGEvalSymbolTable methods
DGEvalSymbolDesc *DGEvalSymbolTable::findSymbol(string name)
{
   auto                 c=symbols.begin();
   size_t               cnt=symbols.size();

   for (size_t i=0;i<cnt;i++, c++)
      if ((*c)->name.compare(name)==0)
         return *c;

   return nullptr;
}

DGEvalSymbolDesc *DGEvalSymbolTable::addSymbol(string name, DGTypeDesc pType)
{
   DGEvalSymbolDesc *node=findSymbol(name);

   if (node==nullptr)
   {
      node=new DGEvalSymbolDesc(name, pType);
      symbols.push_back(node);
      node->ndx=(int)symbols.size()-1;
   }
   else
      node=nullptr;

   return node;
}

void DGEvalSymbolTable::writeAsJSON(ostream *os)
{
   auto                 c=symbols.begin();
   size_t               cnt=symbols.size();

   (*os)<<"[";

   for (size_t i=0;i<cnt;i++, c++)
   {
      if (i>0)
         (*os) <<", ";

      (*os) <<"{"
            <<"\"name\": \""<<(*c)->name<<"\""
            <<", \"type\": \""<<DGEval::typeStr[(int)(*c)->type.type]<<"\""
            <<", \"dim\": "<<to_string((*c)->type.dim)
            <<"}";
   }

   (*os)<<"]";
}

// DGEval methods

/* Scanner for dependencies */
void DGEval::scanDependencies()
{
   for (DGEvalStatementNode *statement=statements->head;statement!=nullptr;statement=statement->next)
   {
      statementInScan=statement;
      statement->addExplicitDependencies();
      scanDependencies(statement, nullptr, statement->exp);
   }
   buildStatementDependencyLists();
   draftIndependents();

   for (DGEvalStatementNode *statement=statements->head;statement!=nullptr;statement=statement->next)
   {
      messageSet.appendMessage(statement->lineNumber, "Statement in circular reference.", DGEvalMsgSeverity::Error);
   }
}

void DGEval::buildStatementDependencyLists()
{
   for (DGEvalStatementNode *statement=statements->head;statement!=nullptr;statement=statement->next)
   {
      int dc=statement->symbolDependency->size();

      for (int j=0;j<dc;j++)
      {
         string *symbol=statement->symbolDependency->identifierAt(j);

         for (DGEvalStatementNode *potentialPredecessor=statements->head;potentialPredecessor!=nullptr;potentialPredecessor=potentialPredecessor->next)
         {
            if (potentialPredecessor->doesDefine(symbol))
               statement->addPredecessor(potentialPredecessor);
         }
      }
   }
}

void DGEval::scanDependencies(DGEvalStatementNode *statement, DGEvalExpNode *parentNode, DGEvalExpNode *node)
{
   DGEvalExpNode *left=node->left,
                 *right=node->right;

   if (left!=nullptr)
      scanDependencies(statement, node, left);

   if (right!=nullptr)
      scanDependencies(statement, node, right);

   switch (node->opCode)
   {
      case INSID:
         DGEvalFuncDesc *f=DGEval::findLibFunction(*node->stringValue);

         if (f==nullptr)
         {
            if (parentNode && parentNode->opCode==OP_ASSIGN && parentNode->left==node)
               statement->addDefining(node->stringValue);
            else
               statement->addDependency(node->stringValue);
         }
         break;
   }
}

void DGEval::scanCalculateTypes()
{
   for (DGEvalStatementNode *statement=draftedStatements->head;statement!=nullptr;statement=statement->next)
   {
      statementInScan=statement;
      scanCalculateTypes(nullptr, statement->exp);
   }
}

void DGEval::scanCalculateTypes(DGEvalExpNode *parentNode, DGEvalExpNode *node)
{
   DGEvalExpNode *left=node->left,
                 *right=node->right;
   int            ec=0;

   if (left!=nullptr)
   {
      scanCalculateTypes(node, left);
      node->functionCallCount+=node->left->functionCallCount;
      node->assignmentCount+=node->left->assignmentCount;
   }

   if (right!=nullptr)
   {
      scanCalculateTypes(node, right);
      node->functionCallCount+=node->right->functionCallCount;
      node->assignmentCount+=node->right->assignmentCount;
   }

   switch (node->opCode)
   {
      case OP_COMMA:
         node->type=node->left->type;
         break;
      case OP_BOR:
      case OP_BAND:
         if ((left->type.dim==0 ) && (right->type.dim==0))
         {
            if (left->type.type==right->type.type && left->type.type==DGEvalType::DGBoolean)
            {
               node->type=left->type;
            }
            else
               messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> can be applied to booleans only.", DGEvalMsgSeverity::Error);
         }
         else
            messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> cannot be applied to array operand(s).", DGEvalMsgSeverity::Error);
         break;
      case OP_EQ:
      case OP_NEQ:
         if (left->type.type==right->type.type)
         {
            node->type.type=DGEvalType::DGBoolean;
         }
         else
            messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> can be applied to identical types only.", DGEvalMsgSeverity::Error);
         break;
      case OP_LTE:
      case OP_LT:
      case OP_GTE:
      case OP_GT:
         if ((left->type.dim==0 ) && (right->type.dim==0))
         {
            if (left->type.type==right->type.type)
            {
               if (left->type.type!=DGEvalType::DGBoolean)
                  node->type.type=DGEvalType::DGBoolean;
               else
                  messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> cannot be applied to booleans.", DGEvalMsgSeverity::Error);
            }
            else
               messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> can be applied to identical types only.", DGEvalMsgSeverity::Error);
         }
         else
            messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> cannot be applied to array operand(s).", DGEvalMsgSeverity::Error);
         break;
      case OP_SUB:
      case OP_MUL:
      case OP_DIV:
         if ((left->type.dim==0 ) && (right->type.dim==0))
         {
            if (left->type.type==right->type.type)
            {
               if (left->type.type==DGEvalType::DGNumber)
                  node->type=left->type;
               else
                  messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> can be applied to numbers or strings only.", DGEvalMsgSeverity::Error);
            }
            else
               messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> can be applied to identical types only.", DGEvalMsgSeverity::Error);
         }
         else
            messageSet.appendMessage(node->lineNumber, "Operator <"+DGEval::opStr[node->opCode]+"> cannot be applied to array operand(s).", DGEvalMsgSeverity::Error);
         break;
      case OP_ADD:
         if ((left->type.dim==0 ) && (right->type.dim==0))
         {
            if (left->type.type==DGEvalType::DGNumber && right->type.type==DGEvalType::DGNumber)
            {
               node->type=left->type;
            }
            else if (left->type.type==DGEvalType::DGString && right->type.type==DGEvalType::DGString)
            {
               node->type=left->type;
            }
            else if (left->type.type==DGEvalType::DGString && right->type.type==DGEvalType::DGNumber)
            {
               if (left->opCode==CONST && right->opCode==CONST)
               {
                  node->right->stringValue=new string(to_string(node->right->doubleValue));
               }
               else
               {
                  node->right=new DGEvalExpNode(node->lineNumber, node->right, LRT, nullptr);
                  node->right->idNdx=5;
               }
               node->right->type.type=DGEvalType::DGString;
               node->type=left->type;
            }
            else if (left->type.type==DGEvalType::DGNumber && right->type.type==DGEvalType::DGString)
            {
               if (left->opCode==CONST && right->opCode==CONST)
               {
                  node->left->stringValue=new string(to_string(node->left->doubleValue));
               }
               else
               {
                  node->left=new DGEvalExpNode(node->lineNumber, node->left, LRT, nullptr);
                  node->left->idNdx=5;
               }
               node->left->type.type=DGEvalType::DGString;
               node->type=right->type;
            }
            else
               messageSet.appendMessage(node->lineNumber, "Operator (+) cannot be applied to the specified operands.", DGEvalMsgSeverity::Error);
         }
         else
         {
            if (left->type.dim-right->type.dim==1 && left->type.type==right->type.type)
            {
               node->type=left->type;
            }
            else
               messageSet.appendMessage(node->lineNumber, "Operator (+) cannot be applied to the specified operands.", DGEvalMsgSeverity::Error);
         }
         break;
      case OP_COND:
         if ((left->type.dim==0) && (left->type.type==DGEvalType::DGBoolean))
         {
            if (right->left->type==right->right->type)
            {
               node->type=
               right->type=right->left->type;
            }
            else
               messageSet.appendMessage(node->lineNumber, "Ternary operator's 2nd and 3rd operands must be of the same type.", DGEvalMsgSeverity::Error);
         }
         else
            messageSet.appendMessage(node->lineNumber, "First operand of the ternary operator must be boolean.", DGEvalMsgSeverity::Error);
         break;
      case OP_MINUS:
         if ((left->type.dim==0) && (left->type.type==DGEvalType::DGNumber))
         {
            node->type=left->type;
         }
         else
            messageSet.appendMessage(node->lineNumber, "Minus operator can be applied to numbers only.", DGEvalMsgSeverity::Error);
         break;
      case OP_NOT:
         if ((left->type.dim==0) && (left->type.type==DGEvalType::DGBoolean))
         {
            node->type=left->type;
         }
         else
            messageSet.appendMessage(node->lineNumber, "Not operator can be applied to booleans only.", DGEvalMsgSeverity::Error);
         break;
      case OP_CALL:
         if (left->opCode==INSID)
         {
            node->functionCallCount++;
            DGEvalFuncDesc *f=DGEval::findLibFunction(*left->stringValue);

            if (f!=nullptr)
            {
               vector<DGEvalExpNode *>   *v=expressionPartVector(right);
               int                        pc=v->size();

               node->left->idNdx=f-DGEval::lib;
               node->type=f->types[0];
               if (pc==f->paraCount)
               {
                  DGTypeDesc    *td=f->types+1;

                  for (int i=0;i<pc;i++, td++)
                  {
                     if ((*v)[pc-i-1]->type!=*td)
                     {
                        messageSet.appendMessage(node->lineNumber,
                              string("Type of parameter ")+to_string(i+1)+" of function <"+*left->stringValue+"> must be "+DGEval::typeStr[(int)td->type]+"["+to_string(td->dim)+"].",
                              DGEvalMsgSeverity::Error);
                        ec++;
                     }
                  }
               }
               else
               {
                  messageSet.appendMessage(node->lineNumber, "Function <"+*left->stringValue+"> requires "+to_string(f->paraCount)+" parameters. "+to_string(pc)+" supplied.", DGEvalMsgSeverity::Error);
                  ec++;
               }
               if (ec==0)
                  node->pVector=v;
               else
                  delete v;
            }
            else
               messageSet.appendMessage(node->lineNumber, "Function <"+*left->stringValue+"> not found.", DGEvalMsgSeverity::Error);

         }
         else
            messageSet.appendMessage(node->lineNumber, "Call operator can accept IDs as callee.", DGEvalMsgSeverity::Error);
         break;
      case OP_AA:
         if (left->type.dim>0)
         {
            vector<DGEvalExpNode *>   *v=expressionPartVector(right);
            int                        pc=v->size();

            if (pc==1)
            {
               if ((*v)[0]->type.type==DGEvalType::DGNumber && (*v)[0]->type.dim==0)
               {
                  node->type=left->type;
                  node->type.dim--;
               }
               else
                  messageSet.appendMessage(node->lineNumber, "Array index must be a number.", DGEvalMsgSeverity::Error);
            }
            else
               messageSet.appendMessage(node->lineNumber, "Array access must have one index.", DGEvalMsgSeverity::Error);

            delete v;
         }
         else
            messageSet.appendMessage(node->lineNumber, "Array access can be applied to arrays only.", DGEvalMsgSeverity::Error);
         break;
      case OP_ASSIGN:
         if (left->opCode==INSID)
         {
            DGEvalFuncDesc *f=DGEval::findLibFunction(*left->stringValue);
            if (f==nullptr)
            {
               DGEvalSymbolDesc *v=symbolTable.findSymbol(*left->stringValue);
               if (v==nullptr)
               {
                  node->type=
                  node->left->type=right->type;
                  v=symbolTable.addSymbol(*left->stringValue, node->type);
                  node->idNdx=v->ndx;
                  node->assignmentCount++;
               }
               else
                  messageSet.appendMessage(node->lineNumber, "Cannot redefine symbol <"+*left->stringValue+">.", DGEvalMsgSeverity::Error);
            }
            else
               messageSet.appendMessage(node->lineNumber, "Assignment to a runtime library function <"+*left->stringValue+">.", DGEvalMsgSeverity::Error);
         }
         else
            messageSet.appendMessage(node->lineNumber, "Assignment lvalue can be an identifier only.", DGEvalMsgSeverity::Error);
         break;
      case INSID:
         if (parentNode==nullptr || node!=parentNode->left || parentNode->opCode!=OP_ASSIGN)
         {
            DGEvalFuncDesc *f=DGEval::findLibFunction(*node->stringValue);
            if (f==nullptr)
            {
               DGEvalSymbolDesc *v=symbolTable.findSymbol(*node->stringValue);
               if (v!=nullptr)
               {
                  node->idNdx=v->ndx;
                  node->type=v->type;
               }
               else
                  messageSet.appendMessage(node->lineNumber, "Unresolved symbol <"+*node->stringValue+">.", DGEvalMsgSeverity::Error);
            }
            else
            {
               if (parentNode->opCode!=OP_CALL)
                  messageSet.appendMessage(node->lineNumber, "Function reference must be with a call operator <"+*node->stringValue+">.", DGEvalMsgSeverity::Error);

               // Keep type to help type evaluation and to suppress additional errors.
               node->type=f->types[0];
            }
         }
         break;
      case LRT:
         node->type.type=DGEvalType::DGArray;
         vector<DGEvalExpNode *>   *v=expressionPartVector(left);
         int cnt=v->size();
         DGEvalExpNode *firstNode=(*v)[0];

         node->type=firstNode->type;
         node->type.dim++;

         for (int i=1;i<cnt;i++)
            if (firstNode->type!=(*v)[i]->type)
            {
               messageSet.appendMessage((*v)[i]->lineNumber, "Array literal member must of the same type.", DGEvalMsgSeverity::Error);
               ec++;
               break;
            }
         if (ec==0)
            node->pVector=v;
         else
            delete v;
         break;
   }
}

vector<DGEvalExpNode *> *DGEval::expressionPartVector(DGEvalExpNode *node)
{
   vector<DGEvalExpNode *> *retVal=new vector<DGEvalExpNode *>();

   if (node!=nullptr)
   {
      while (node->opCode==OP_COMMA)
      {
         retVal->push_back(node->right);
         node=node->left;
      }

      retVal->push_back(node);
   }

   return retVal;
}

void DGEval::draftIndependents()
{
   int draftCount;

   do
   {
      DGEvalStatementNode *n;

      draftCount=0;
      for (DGEvalStatementNode *statement=statements->head;statement!=nullptr;statement=n)
      {
         n=statement->next;

         if (statement->statementDependencyList==nullptr)
         {
            for (DGEvalStatementNode *otherStatement=statements->head;otherStatement!=nullptr;otherStatement=otherStatement->next)
               if (otherStatement!=statement)
                  otherStatement->removePredecessor(statement);

            statements->remove(statement);
            statement->next=
            statement->prev=nullptr;
            draftedStatements->append(statement);
            draftCount++;
         }
      }
   }
   while (draftCount>0);
}

void DGEval::writeAsJSON(ostream *os)
{
   (*os)<<"{\"circularStatements\": ";
   statements->writeAsJSON(os);
   (*os)<<", \"symbols\": ";
   symbolTable.writeAsJSON(os);
   (*os)<<", \"executablestatements\": ";
   draftedStatements->writeAsJSON(os);

   if (ic!=nullptr)
   {
      (*os)<<", \"ic\": ";
      ic->writeAsJSON(os);
   }

   (*os)<<", \"messages\": ";
   messageSet.writeAsJSON(os);
   (*os)<<"}";
}

// DGEvalStatementNode
void DGEvalStatementNode::addDefining(string *symbol)
{
   defining->appendUnique(symbol);
}

void DGEvalStatementNode::addDependency(string *symbol)
{
   symbolDependency->appendUnique(symbol);
}

void DGEvalStatementNode::addExplicitDependencies()
{
}

bool DGEvalStatementNode::doesDefine(string *identifier)
{
   int cnt=defining->size();

   for (int i=0;i<cnt;i++)
      if (defining->identifierAt(i)->compare(*identifier)==0)
         return true;

   return false;
}

void DGEvalStatementNode::addPredecessor(DGEvalStatementNode *statement)
{
   DGEvalDependencyEntry *s=statementDependencyList;

   statement->asPredecessorCount++;
   while (s)
   {
      if (s->statement==statement)
         return ;
      s=s->next;
   }

   s=new DGEvalDependencyEntry{statement, statementDependencyList};

   statementDependencyList=s;
}

bool DGEvalStatementNode::removePredecessor(DGEvalStatementNode *statement)
{
   DGEvalDependencyEntry *s=statementDependencyList,
                         *p=nullptr;

   while (s!=nullptr)
   {
      if (s->statement==statement)
      {
         if (p==nullptr)
            statementDependencyList=s->next;
         else
            p->next=s->next;
         return true;
      }
      p=s;
      s=s->next;
   }
   return false;
}

// DGEvalExpStatement
void DGEvalExpStatement::writeAsJSON(ostream *os)
{
   (*os)<<"{\"lineNumber\": "+to_string(lineNumber)+", \"nodeType\": \"expression statement\", \"expression\": ";
   exp->writeAsJSON(os);
   (*os)<<"}";
}

// DGEvalExpStatement
void DGEvalWaitStatement::addExplicitDependencies()
{
   symbolDependency->appendUnique(identifierList);
}

void DGEvalWaitStatement::writeAsJSON(ostream *os)
{
   (*os)<<"{\"lineNumber\": "+to_string(lineNumber)+", \"nodeType\": \"wait statement\", \"expression\": ";
   exp->writeAsJSON(os);
   (*os)<<", \"idList\": ";
   identifierList->writeAsJSON(os);
   (*os)<<"}";
}

// DGEvalExpNode
void DGEvalExpNode::writeAsJSON(ostream *os)
{
   (*os)<<"{";

   (*os) <<"\"lineNumber\": "+to_string(lineNumber)+", \"nodeType\": \"expression node\", \"opCode\": "
         <<opCode<<", \"mnemonic\": \""<<DGEval::opStr[opCode]
         <<"\", \"typeCode\": "<<((int)type.type)<<", \"type\": \""<<DGEval::typeStr[(int)type.type]
         <<"\", \"dim\": "<<type.dim
         <<", \"idNdx\": "<<idNdx;

   if (opCode==CONST)
   {
      if (type.dim==0)
      {
         switch (type.type)
         {
            case DGEvalType::DGString:
               (*os)<<", \"stringValue\": ";
               DGEval::writeStrValue(os,stringValue);
               break;
            case DGEvalType::DGNumber:
               (*os)<<", \"numberValue\": \""<<doubleValue<<"\"";
               break;
            case DGEvalType::DGBoolean:
               (*os)<<", \"numberValue\": \""<<boolValue<<"\"";
               break;
            default:
               break;
         }
      }
   }

   if (opCode==INSID)
   {
      (*os)<<", \"id\": \""<<*stringValue<<"\"";
   }

   if (left!=nullptr)
   {
      (*os)<<", \"left\":";
      left->writeAsJSON(os);
   }

   if (right!=nullptr)
   {
      (*os)<<", \"right\":";
      right->writeAsJSON(os);
   }

   (*os)<<"}";
}

// DGEvalIdentifierList
void DGEvalIdentifierList::append(string *identifier)
{
   identifiers.push_back(identifier);
}

bool DGEvalIdentifierList::appendUnique(string *symbol)
{
   auto                 c=identifiers.begin();
   size_t               cnt=identifiers.size();

   for (size_t i=0;i<cnt;i++, c++)
      if ((*c)->compare(*symbol)==0)
         return false;

   identifiers.push_back(symbol);
   return true;
}

bool DGEvalIdentifierList::appendUnique(DGEvalIdentifierList *otherList)
{
   bool retVal=true;
   int  cnt=otherList->size();

   for (int i=0;i<cnt;i++)
      retVal=retVal && appendUnique(otherList->identifierAt(i));

   return retVal;
}

int DGEvalIdentifierList::size()
{
   return identifiers.size();
}

string *DGEvalIdentifierList::identifierAt(int ndx)
{
   return identifiers[ndx];
}


void DGEvalIdentifierList::writeAsJSON(ostream *os)
{
   (*os)<<"[";

   auto                 c=identifiers.begin();
   size_t               cnt=identifiers.size();

   if (cnt>0)
   {
      (*os)<<"\""<<**c<<"\"";
      c++;
      for (size_t i=1;i<cnt;i++, c++)
         (*os)<<", \""<<**c<<"\"";
   }

   (*os)<<"]";
}

// DGEvalStatementList
void DGEvalStatementList::append(DGEvalStatementNode *statement)
{
   statement->prev=tail;
   if (tail!=nullptr)
      tail->next=statement;
   else
      head=statement;
   tail=statement;
}

void DGEvalStatementList::remove(DGEvalStatementNode *statement)
{
   if (statement->next!=nullptr)
      statement->next->prev=statement->prev;
   else
      tail=statement->prev;

   if (statement->prev!=nullptr)
      statement->prev->next=statement->next;
   else
      head=statement->next;
}

void DGEvalStatementList::writeAsJSON(ostream *os)
{
   (*os)<<"[";

   if (head!=nullptr)
   {
      head->writeAsJSON(os);
      for (DGEvalStatementNode *node=head->next;node!=nullptr;node=node->next)
      {
        (*os)<<", ";
        node->writeAsJSON(os);
      }
   }
   (*os)<<"]";
}
