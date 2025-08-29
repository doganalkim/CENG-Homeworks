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
   "jt",
   "id",
   "const",
   "lrt",
   "pop"
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

DGEvalFuncDesc DGEval::lib[]=
{
   {"stddev", (void *)DGEval::stddev, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"mean", (void *)DGEval::mean, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"count", (void *)DGEval::count, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"min", (void *)DGEval::min, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"max", (void *)DGEval::max, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 1}}},
   {"sin", (void *)DGEval::sin, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"cos", (void *)DGEval::cos, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"tan", (void *)DGEval::tan, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"pi", (void *)DGEval::pi, false, 0, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"atan", (void *)DGEval::atan, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"asin", (void *)DGEval::asin, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"acos", (void *)DGEval::acos, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"exp", (void *)DGEval::exp, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"ln", (void *)DGEval::ln, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"print", (void *)DGEval::print, false, 1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGString, 0}}},
   {"random", (void *)DGEval::random, false, 1,{{DGEvalType::DGNumber, 0},{DGEvalType::DGNumber, 0}}},
   {"len", (void *)DGEval::len, false, 1, {{DGEvalType::DGNumber, 0},{DGEvalType::DGString, 0}}},
   {"right", (void *)DGEval::right, true, 2, {{DGEvalType::DGString, 0},{DGEvalType::DGString, 0},{DGEvalType::DGNumber, 0}}},
   {"left", (void *)DGEval::left, true, 2, {{DGEvalType::DGString, 0},{DGEvalType::DGString, 0},{DGEvalType::DGNumber, 0}}}
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
   {
      if (type.dim==0)
      {
         if (type.type==DGEvalType::DGString)
            delete stringValue;
      }
   }

   if (pVector!=nullptr)
      delete pVector;

   if (left!=nullptr)
      delete left;

   if (right!=nullptr)
      delete right;
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
/*
DGEvalStatementList::DGEvalStatementList(DGEvalStatementNode *statement)
{
   head=
   tail=statement;
}
*/

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
   optimization=pParser->optimization;

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

   if (ic!=nullptr)
      delete ic;
}

void DGEval::writeStrValue(ostream *os, string *strValue)
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

DGEvalFuncDesc *DGEval::libFunctionAt(int ndx)
{
   return lib+ndx;
}

void DGEval::registerStringObject(string *str)
{
   stringVectorToClean.push_back(str);
}

void DGEval::registerDGArrayObject(DGEvalArray *arr)
{
   arrayVectorToClean.push_back(arr);
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

DGEvalSymbolDesc *DGEvalSymbolTable::symbolAt(int ndx)
{
   return symbols[ndx];
}

int DGEvalSymbolTable::symbolCount()
{
   return (int)symbols.size();
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

int DGEvalMsgSet::messageCount(DGEvalMsgSeverity t)
{
   size_t               cnt=messages.size();
   int                  retVal=0;

   for (size_t i=0;i<cnt;i++)
      if (messages[i]->s==t)
         retVal++;

   return retVal;
}


DGEvalICInst::DGEvalICInst(int pOpCode, int pP1)
{
   opCode=pOpCode;
   p1=pP1;
   type.type=DGEvalType::DGNone;
   type.dim=0;
}
DGEvalICInst::DGEvalICInst(int pOpCode, int pP1, DGTypeDesc pType)
{
   opCode=pOpCode;
   p1=pP1;
   type=pType;
}

DGEvalICInst::DGEvalICInst()
{
}

DGEvalICInst::~DGEvalICInst()
{
}

DGEvalCodeLocation::DGEvalCodeLocation()
{
}

DGEvalCodeLocation::DGEvalCodeLocation(DGEvalIC *pIc, DGEvalICInst i)
{
   ic=pIc;
   inst=i;
   relocation=0;
}

DGEvalCodeLocation::~DGEvalCodeLocation()
{
   if (ij!=nullptr)
      delete ij;
}

void DGEvalCodeLocation::registerPredecessor(int source)
{
   if (ij==nullptr)
      ij=new vector<int>();

   int cnt=ij->size();

   for (int i=0;i<cnt;i++)
      if ((*ij)[i]==source)
         return;

   ij->push_back(source);
}

bool DGEvalCodeLocation::ivHasNdx(vector<int> *v, int ndx)
{
   int cnt=v->size(),
       i,
       *p=v->data();

   for (i=0;i<cnt;i++, p++)
      if (*p==ndx)
         break;

   return i<cnt;
}

void DGEvalCodeLocation::ivAppend(vector<int> *v, int ndx)
{
   if (!ivHasNdx(v, ndx))
      v->push_back(ndx);
}
void DGEvalCodeLocation::ivAppend(vector<int> *v, vector<int> *s)
{
   int cnt=s->size();

   for (int i=0;i<cnt;i++)
      ivAppend(v,(*s)[i]);
}

void DGEvalCodeLocation::predecessors(vector<int> *accumulator)
{
   int ndx=ic->codeLocationIndex(this);

   if (ij!=nullptr)
      ivAppend(accumulator,ij);

   if (ndx>0)
   {
      DGEvalCodeLocation *precedingCL=(this-1);

      if (precedingCL->inst.opCode!=JMP)
         ivAppend(accumulator, ndx-1);
   }
}

vector<int> *DGEvalCodeLocation::predecessors()
{
   vector<int> *retVal=new vector<int>();

   predecessors(retVal);

   return retVal;
}

void DGEvalIC::collectImmediateJumps()
{
   DGEvalCodeLocation  *cl=code.data();
   int                  cnt=code.size();

   for (int i=0;i<cnt;i++, cl++)
   {
      DGEvalICInst  *inst=&cl->inst;
      int            opCode=inst->opCode;

      if (opCode==JMP || opCode==JF || opCode==JT)
         code[inst->p1].registerPredecessor(i);
   }
}

DGEvalCodePathWindow::DGEvalCodePathWindow()
{
}

DGEvalCodePathWindow::~DGEvalCodePathWindow()
{
   int               cnt=path.size();
   DGEvalCodePath  **p=path.data();

   for (int i=0;i<cnt;i++, p++)
      delete *p;
}

void DGEvalCodePathWindow::build(DGEvalIC *ic, int codeLocationIndex, int windowSize)
{
   int *builder=new int[windowSize];

   build(ic, codeLocationIndex, windowSize, 0, builder);

   delete []builder;
}

void DGEvalCodePathWindow::build(DGEvalIC *ic, int codeLocationIndex, int windowSize, int depth, int *builder)
{
   DGEvalCodeLocation *cl=ic->codeLocationAt(codeLocationIndex);
   vector<int>   *t=cl->predecessors();
   int            cnt=t->size();

   builder[depth]=codeLocationIndex;
   if (cnt>0 && depth<windowSize-1)
   {
      for (int i=0;i<cnt;i++)
         build(ic, (*t)[i], windowSize, depth+1, builder);
   }
   else
   {
      DGEvalCodePath *p=new vector<int>();

      for (int i=depth;i>=0;i--)
         p->push_back(builder[i]);

      path.push_back(p);
   }
}

DGEvalIC::DGEvalIC()
{
}

DGEvalIC::~DGEvalIC()
{
}

DGEvalICInst *DGEvalIC::emitIC(int opCode, int p1)
{
   code.push_back(DGEvalCodeLocation(this, DGEvalICInst(opCode, p1)));
   return &code[code.size()-1].inst;
}

DGEvalICInst *DGEvalIC::emitIC(int opCode, int p1, DGTypeDesc type)
{
   code.push_back(DGEvalCodeLocation(this, DGEvalICInst(opCode, p1, type)));
   return &code[code.size()-1].inst;
}

DGEvalICInst *DGEvalIC::instructionAt(int ndx)
{
   if (ndx>=0 && ndx<(int)code.size())
      return &code[ndx].inst;

   return nullptr;
}

DGEvalCodeLocation *DGEvalIC::codeLocationAt(int ndx)
{
   if (ndx>=0 && ndx<(int)code.size())
      return &code[ndx];

   return nullptr;
}

bool DGEvalIC::markRemoval(int ndx, int count)
{
   bool retVal=ndx>=0 && ndx+count<=(int)code.size();

   if (retVal)
   {

      int   e=ndx+count;

      for (int i=ndx;i<e;i++)
         code[i].inst.opCode=OP_NOP;
   }

   return retVal;
}

int DGEvalIC::applyRemoval()
{
   int                     cnt=code.size(),
                           offset=0;
   DGEvalCodeLocation     *cl=code.data();

   for (int i=0;i<cnt;i++, cl++)
   {
      cl->relocation=offset;
      if (cl->inst.opCode==OP_NOP)
         offset--;
   }

   cl=code.data();
   for (int i=0;i<cnt;i++, cl++)
   {
      DGEvalICInst  *inst=&cl->inst;

      switch (inst->opCode)
      {
         case JMP:
         case JF:
            inst->p1+=code[inst->p1].relocation;
            break;
      }

      if (cl->ij!=nullptr)
      {
         vector<int> *ij=cl->ij;
         int *p=ij->data(),
              cnt=ij->size();

         for (int i=0;i<cnt;i++, p++)
            *p=code[*p].relocation;
      }

      int r=code[i].relocation;
      code[i+r].inst=*inst;
   }

   code.resize(cnt+offset);

   return offset;
}

int DGEvalIC::instCount()
{
   return code.size();
}

int DGEvalIC::codeLocationIndex(DGEvalCodeLocation *cl)
{
   return cl-code.data();
}

void DGEvalIC::writeReadable(ostream *outStream)
{
   char buf[128];
   int cnt=code.size();

   for (int i=0;i<cnt;i++)
   {
      DGEvalICInst *inst=&code[i].inst;

      sprintf(buf,"%5.5d %-8s %2d type:[%s:%d] ", i, DGEval::opStr[inst->opCode].c_str(), inst->p1, DGEval::typeStr[(int)inst->type.type].c_str(), inst->type.dim);

      (*outStream) << buf;

      if (inst->opCode==CONST && inst->type.dim==0)
      {
         switch (inst->type.type)
         {
            case DGEvalType::DGNumber:
               (*outStream) << inst->numConstant;
               break;
            case DGEvalType::DGBoolean:
               (*outStream) << inst->boolConstant;
               break;
            case DGEvalType::DGString:
               DGEval::writeStrValue(outStream, inst->strConstant);
               break;
            default: ;
         }
      }

      if (inst->opCode==INSID || inst->opCode==OP_ASSIGN)
      {
         if (inst->strConstant!=nullptr)
            DGEval::writeStrValue(outStream, inst->strConstant);
      }

      if (inst->opCode==LRT)
      {
         if (inst->p1==3)
            DGEval::writeStrValue(outStream, inst->strConstant);
         else
            (*outStream) << inst->numConstant;
      }
      if (inst->opCode==OP_CALL)
         (*outStream) << "\"" << DGEval::libFunctionAt((int)inst->numConstant)->name << "\" @"<< (int)inst->numConstant;

      (*outStream) << endl;
   }
}

DGEvalExpNode *DGEval::destroyExpTree(DGEvalExpNode *node)
{
   delete node;

   return nullptr;
}

void DGEval::transformStrConstToLRT(DGEvalExpNode *node)
{
   if (node!=nullptr && node->type.type==DGEvalType::DGString && node->opCode==CONST)
   {
      node->opCode=LRT;
      node->idNdx=3;
   }
}

DGEvalArrayString::DGEvalArrayString()
   :DGEvalArrayType<string *>({DGEvalType::DGString, 0})
{
}

DGEvalArrayString::DGEvalArrayString(string **base, int count)
   :DGEvalArrayType<string *>({DGEvalType::DGString, 0})
{
   int n=count-1;
   for (int i=0;i<count;i++, n--)
      arr->push_back(base[n]);
}

DGEvalArrayString::~DGEvalArrayString()
{
}

bool DGEvalArrayString::areEqualElements(string *p1, string *p2)
{
   return p1->compare(*p2)==0;
}

DGEvalArrayDouble::DGEvalArrayDouble()
   :DGEvalArrayType<double>({DGEvalType::DGNumber, 0})
{
}

DGEvalArrayDouble::DGEvalArrayDouble(double *base, int count)
   :DGEvalArrayType<double>({DGEvalType::DGNumber, 0})
{
   int n=count-1;
   for (int i=0;i<count;i++, n--)
      arr->push_back(base[n]);
}

DGEvalArrayDouble::~DGEvalArrayDouble()
{
}

bool DGEvalArrayDouble::areEqualElements(double p1, double p2)
{
   return p1==p2;
}

double DGEvalArrayDouble::stddev()
{
   double count=arr->size();

   if (count==0)
      return 0;

   double sumx=0,
          sumx2=0;

   for (int i=0;i<count;i++)
   {
      double v=(*arr)[i];
      sumx+=v;
      sumx2+=v*v;
   }

   double m = sumx / count;
   double variance = sumx2 / count - m * m;

   return sqrt(variance);
}

double DGEvalArrayDouble::mean()
{
   double count=arr->size();

   if (count==0)
      return 0;

   double sumx=0;

   for (int i=0;i<count;i++)
      sumx+=(*arr)[i];

   return sumx/count;
}

double DGEvalArrayDouble::count()
{
   return arr->size();
}

double DGEvalArrayDouble::min()
{
   double          count=arr->size();

   if (count==0)
      return 0;

   double min=(*arr)[0];

   for (int i=1;i<count;i++)
   {
      double v=(*arr)[i];
      if (v<min)
         min=v;
   }

   return min;
}

double DGEvalArrayDouble::max()
{
   double          count=arr->size();

   if (count==0)
      return 0;

   double max=(*arr)[0];

   for (int i=1;i<count;i++)
   {
      double v=(*arr)[i];
      if (v>max)
         max=v;
   }

   return max;
}

DGEvalArrayBool::DGEvalArrayBool()
   :DGEvalArrayType<bool>({DGEvalType::DGBoolean, 0})
{
}

DGEvalArrayBool::DGEvalArrayBool(int64_t *base, int count)
   :DGEvalArrayType<bool>({DGEvalType::DGBoolean, 0})
{
   int n=count-1;
   for (int i=0;i<count;i++, n--)
      arr->push_back((bool)base[n]);
}

DGEvalArrayBool::~DGEvalArrayBool()
{
}

bool DGEvalArrayBool::areEqualElements(bool p1, bool p2)
{
   return p1==p2;
}

DGEvalArrayArray::DGEvalArrayArray(DGTypeDesc td)
   :DGEvalArrayType<DGEvalArray *>(td)
{
}

DGEvalArrayArray::DGEvalArrayArray(DGTypeDesc td, DGEvalArray **base, int count)
   :DGEvalArrayType<DGEvalArray *>(td)
{
   int n=count-1;
   for (int i=0;i<count;i++, n--)
      arr->push_back(base[n]);
}

DGEvalArrayArray::~DGEvalArrayArray()
{
}

bool DGEvalArrayArray::areEqualElements(DGEvalArray *p1, DGEvalArray *p2)
{
   return p1->equalsToOther(p2);
}

uint64_t DGEval::arrayelement(DGEval *dgEval, DGEvalArray *arr, int64_t index)
{
   uint64_t             retVal=0;
   double               v;
   DGEvalArrayBool     *boolArray;
   DGEvalArrayString   *stringArray;
   DGEvalArrayDouble   *doubleArray;

   if (arr->type.dim==0)
      switch (arr->type.type)
      {
         case DGEvalType::DGBoolean:
            boolArray=(DGEvalArrayBool *)arr;

            dgEval->exception=index<0 || index>=(int64_t)boolArray->arr->size();
            if (!dgEval->exception)
               retVal=(uint64_t)(*boolArray->arr)[index];
            break;
         case DGEvalType::DGString:
            stringArray=(DGEvalArrayString *)arr;

            dgEval->exception=index<0 || index>=(int64_t)stringArray->arr->size();
            if (!dgEval->exception)
               retVal=(uint64_t)(*stringArray->arr)[index];
            break;
         case DGEvalType::DGNumber:
            doubleArray=((DGEvalArrayDouble *)arr);

            dgEval->exception=index<0 || index>=(int64_t)doubleArray->arr->size();
            if (!dgEval->exception)
            {
               v=(*doubleArray->arr)[index];
               retVal=*(uint64_t *)&v;
            }
            break;
         default:
            break;
      }
   else
   {
      DGEvalArrayArray *arrayArray=((DGEvalArrayArray *)arr);

      dgEval->exception=index<0 || index>=(int64_t)arrayArray->arr->size();
      if (!dgEval->exception)
         retVal=(uint64_t)(*arrayArray->arr)[index];
   }

   return retVal;
}

DGEvalArray *DGEval::allocatearray(DGEval *dgEval, DGTypeDesc signature, int itemCount, uint64_t *base)
{
   DGEvalArray *array;


   if (signature.dim==1)
   {
      switch (signature.type)
      {
         case DGEvalType::DGBoolean:
            array=new DGEvalArrayBool((int64_t *)base, itemCount);
            break;
         case DGEvalType::DGString:
            array=new DGEvalArrayString((string **)base, itemCount);
            break;
         case DGEvalType::DGNumber:
            array=new DGEvalArrayDouble((double *)base, itemCount);
            break;
         default:
            break;
      }
   }
   else
      array=new DGEvalArrayArray(signature, (DGEvalArray **)base, itemCount);

   dgEval->registerDGArrayObject(array);

   return array;
}

DGEvalArray *DGEval::appendelement(DGEvalArray *arr, uint64_t para)
{
   double v;

   if (arr->type.dim==0)
      switch (arr->type.type)
      {
         case DGEvalType::DGBoolean:
            ((DGEvalArrayBool *)arr)->arr->push_back((bool)para);
            break;
         case DGEvalType::DGString:
            ((DGEvalArrayString *)arr)->arr->push_back((string *)para);
            break;
         case DGEvalType::DGNumber:
            v=*(double *)&para;
            ((DGEvalArrayDouble *)arr)->arr->push_back(v);
            break;
         default:
            break;
      }
   else
      ((DGEvalArrayArray *)arr)->arr->push_back((DGEvalArrayArray *)para);

   return arr;
}

string *DGEval::allocatestring(DGEval *dgEval, string *s1)
{
   string *retVal=new string(*s1);

   dgEval->registerStringObject(retVal);

   return retVal;
}

string *DGEval::catstring(DGEval *dgEval, string *s1, string *s2)
{
   vector<string *> stringVectorToClean=dgEval->stringVectorToClean;
   string *retVal=new string((*s1)+(*s2));

   dgEval->registerStringObject(retVal);

   return retVal;
}

string *DGEval::number2str(DGEval *dgEval, double n)
{
   string *retVal=new string(to_string(n));

   dgEval->registerStringObject(retVal);

   return retVal;
}

int64_t DGEval::strcmp(string *s1, string *s2, int64_t test)
{
   int   r=0,
         v=s1->compare(*s2);

   switch (test)
   {
      case 0:
         r=v==0;
         break;
      case 1:
         r=v!=0;
         break;
      case 2:
         r=v>0;
         break;
      case 3:
         r=v<0;
         break;
      case 4:
         r=v>=0;
         break;
      case 5:
         r=v<=0;
         break;
   }

   return r;
}

int64_t DGEval::arrcmp(DGEvalArray *arr1, DGEvalArray *arr2)
{
   return arr1->equalsToOther(arr2);
}

int64_t DGEval::postexecutecleanup(DGEval *dgEval)
{
   vector<string *> stringVectorToClean=dgEval->stringVectorToClean;
   vector<DGEvalArray *> arrayVectorToClean=dgEval->arrayVectorToClean;

   int cnt=stringVectorToClean.size();

   for (int i=0;i<cnt;i++)
      delete stringVectorToClean[i];

   cnt=dgEval->arrayVectorToClean.size();

   for (int i=0;i<cnt;i++)
      delete arrayVectorToClean[i];

   return true;
}

int64_t DGEval::checkexception(DGEval *dgEval)
{
   return dgEval->exception;
}
