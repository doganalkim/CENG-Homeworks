using namespace std;
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include "dgevalsup.h"

#include "dgevalsyn.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyParser.h"

int *DGEval::sVector=nullptr;

string DGEval::opStr[]=
{
   "nop",
   "comma",
   "assign",
   "cond",
   "alt",
   "band",
   "bor",
   "eq",
   "neq",
   "lt",
   "lte",
   "gt",
   "gte",
   "add",
   "sub",
   "mul",
   "div",
   "minus",
   "not",
   "aa",
   "call",
   "jmp",
   "jf",
   "id",
   "const"
};

string DGEval::typeStr[]=
{
   "none",
   "number",
   "string",
   "boolean",
   "array"
};

string DGEval::severityStr[]=
{
   "Error",
   "Warning",
   "Info"
};

bool DGTypeDesc::operator == (DGTypeDesc other)
{
   return type==other.type && dim==other.dim;
}
bool DGTypeDesc::operator != (DGTypeDesc other)
{
   return type!=other.type || dim!=other.dim;
}

DGEvalFuncDesc d{"mean",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}};

DGEvalFuncDesc DGEval::lib[]=
{
   {"stddev",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"mean",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"count",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"min",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"max",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"sin",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"cos",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"tan",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"pi",0,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"atan",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"asin",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"acos",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"exp",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"ln",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"print",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGString, 0}}},
   {"random",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"len",1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGString, 0}}},
   {"right",2,{{DGEvalType::DGString, 0},{DGEvalType::DGString, 0},{DGEvalType::DGNumber, 0}}},
   {"left",2,{{DGEvalType::DGString, 0},{DGEvalType::DGString, 0},{DGEvalType::DGNumber, 0}}}
};

DGEvalNode::DGEvalNode(int pLineNumber)
{
   lineNumber=pLineNumber;
}

DGEvalNode::~DGEvalNode()
{
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode)
   :DGEvalNode(pLineNumber)
{
   left=pLeft;
   opCode=pOpCode;
   right=nullptr;
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode, DGEvalExpNode *pRight)
   :DGEvalNode(pLineNumber)
{
   left=pLeft;
   opCode=pOpCode;
   right=pRight;;
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode, DGEvalExpNode *pRight, DGEvalExpNode *pThird)
   :DGEvalNode(pLineNumber)
{
   left=pLeft;
   opCode=pOpCode;
   right=pRight;
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, double value, int pOpCode)
   :DGEvalNode(pLineNumber)
{
   left=
   right=nullptr;
   opCode=pOpCode;
   doubleValue=value;
   type.type=DGEvalType::DGNumber;
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, string *value, int pOpCode)
   :DGEvalNode(pLineNumber)
{
   left=
   right=nullptr;
   opCode=pOpCode;
   stringValue=value;
   if (opCode==CONST)
      type.type=DGEvalType::DGString;
}

DGEvalExpNode::DGEvalExpNode(int pLineNumber, bool value, int pOpCode)
   :DGEvalNode(pLineNumber)
{
   left=
   right=nullptr;
   opCode=pOpCode;
   boolValue=value;
   type.type=DGEvalType::DGBoolean;
}


DGEvalExpNode::DGEvalExpNode(int pLineNumber, DGEvalExpNode *arrayRoot)
   :DGEvalNode(pLineNumber)
{
   left=arrayRoot;
   right=nullptr;
   opCode=CONST;
   type.type=DGEvalType::DGArray;
}

DGEvalExpNode::~DGEvalExpNode()
{
   if (opCode==CONST)
      if (type.dim==0 && type.type==DGEvalType::DGString)
         delete stringValue;

   if (left!=nullptr)
      delete left;

   if (right!=nullptr)
      delete right;
}

void DGEvalNode::writeStrValue(ostream *os, string *strValue)
{
   static  unsigned char const *hd=(unsigned char const *)"0123456789abcdef";
   string  t=string();
   int     l=(int)strValue->length();

   t+='"';
   for (int i=0;i<l;i++)
   {
      char c=(*strValue)[i];

      switch (c)
      {
         case '\n':
            t+="\\n";
            break;
         case '\t':
            t+="\\t";
            break;
         case '\r':
            t+="\\r";
            break;
         case '\\':
            t+="\\\\";
            break;
         default:
            if (c<32)
            {
               t+="\\x";
               t+=hd[c>>4];
               t+=hd[c&0xf];
            }
            else
               t+=(*strValue)[i];
      }
   }

   t+='"';
   (*os) << t;
}

DGEvalStatementNode::DGEvalStatementNode(int pLineNumber, DGEvalExpNode *pExp)
   :DGEvalNode(pLineNumber)
{
   defining=new DGEvalIdentifierList();
   symbolDependency=new DGEvalIdentifierList();
   exp=pExp;
   next=
   prev=nullptr;
}

DGEvalStatementNode::~DGEvalStatementNode()
{
   delete defining;
   delete symbolDependency;
   delete exp;
}

DGEvalExpStatement::DGEvalExpStatement(int pLineNumber, DGEvalExpNode *pExp)
   :DGEvalStatementNode(pLineNumber, pExp)
{
}

DGEvalExpStatement::~DGEvalExpStatement()
{
}

DGEvalWaitStatement::DGEvalWaitStatement(int pNumber, DGEvalExpNode *pExp, DGEvalIdentifierList *pIdentifierList)
   :DGEvalStatementNode(pNumber, pExp)
{
   identifierList=pIdentifierList;
}

DGEvalWaitStatement::~DGEvalWaitStatement()
{
   delete identifierList;
}

DGEvalIdentifierList::DGEvalIdentifierList()
{
}

DGEvalIdentifierList::DGEvalIdentifierList(string *identifier)
{
   identifiers.push_back(identifier);
}

DGEvalIdentifierList::~DGEvalIdentifierList()
{
   // No need to free the identifiers as the identifier strins are
   // referenced by the expression nodes that are also freed.
}


DGEvalStatementList::DGEvalStatementList()
{
}

DGEvalStatementList::DGEvalStatementList(DGEvalStatementNode *statement)
{
   head=
   tail=statement;
}

DGEvalStatementList::~DGEvalStatementList()
{
   DGEvalStatementNode *n;

   for (DGEvalStatementNode *node=head;node!=nullptr;node=n)
   {
      n=node->next;
      delete node;
   }
}



DGEval::DGEval(MyParser *pParser)
{
   parser=pParser;
   statements=new DGEvalStatementList();
   draftedStatements=new DGEvalStatementList();

   if (DGEval::sVector==nullptr)
   {
      int s=sizeof(DGEval::lib)/sizeof(DGEvalFuncDesc);
      DGEval::sVector=new int[s];

      for (int i=0;i<s;i++)
         DGEval::sVector[i]=i;

      qsort(DGEval::sVector, s, sizeof(int), DGEval::compareFDesc);
   }
}

DGEval::~DGEval()
{
   if (statements!=nullptr)
      delete statements;

   if (draftedStatements!=nullptr)
      delete draftedStatements;
}

int DGEval::compareFDesc(const void *v1, const void *v2)
{
   int   i1=*(int*)v1,
         i2=*(int*)v2;

   return DGEval::lib[i1].name.compare(DGEval::lib[i2].name);
}

DGEvalFuncDesc *DGEval::findLibFunction(string fName)
{
   int   p=0,
         q=sizeof(DGEval::lib)/sizeof(DGEvalFuncDesc)-1;

   do
   {
      int               n=(p+q)/2;
      DGEvalFuncDesc   *desc=DGEval::lib+DGEval::sVector[n];
      int               test=desc->name.compare(fName);

      if  (test==0)
         return desc;

      if (test>0)
         q=n-1;
      else
         p=n+1;

   } while (p<=q);

   return nullptr;
}

DGEvalSymbolTable::DGEvalSymbolTable()
{
}

DGEvalSymbolTable::~DGEvalSymbolTable()
{
   auto                 c=symbols.begin();
   size_t               cnt=symbols.size();

   for (size_t i=0;i<cnt;i++, c++)
      delete *c;
}

DGEvalMsgDesc::DGEvalMsgDesc(int pLineNumber, string pMsg, DGEvalMsgSeverity pS)
{
   lineNumber=pLineNumber;
   msg=pMsg;
   s=pS;
}

DGEvalMsgSet::DGEvalMsgSet()
{
}

DGEvalMsgSet::~DGEvalMsgSet()
{
   auto                 c=messages.begin();
   size_t               cnt=messages.size();

   for (size_t i=0;i<cnt;i++, c++)
      delete *c;
}

void DGEvalMsgSet::appendMessage(int pLineNumber, string pMsg, DGEvalMsgSeverity pS)
{
   messages.push_back(new DGEvalMsgDesc(pLineNumber, pMsg, pS));
}

bool DGEvalMsgSet::compareMessageLines(DGEvalMsgDesc *p1, DGEvalMsgDesc *p2)
{
   return p1->lineNumber < p2->lineNumber;
}

void DGEvalMsgSet::sort()
{
   if (messages.size()>0)
      std::sort(messages.begin(), messages.end(), compareMessageLines);
}
