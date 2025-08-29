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

#include <cmath>

#define PI M_PI
#define NUMB_OF_FUNCS 19

#define WINDOW_SIZE 3
#define JMP_LEN 5
#define JE_LEN 6

// Library Start
double DGEval::stddev(DGEvalArrayDouble *array)
{
    return array->stddev();
}

double DGEval::mean(DGEvalArrayDouble *array)
{
    return array->mean();
}

double DGEval::count(DGEvalArrayDouble *array)
{
    return array->count();
}

double DGEval::min(DGEvalArrayDouble *array)
{
    return array->min();
}

double DGEval::max(DGEvalArrayDouble *array)
{
    return array->max();
}

double DGEval::sin(double number)
{
    return std::sin(number);
}

double DGEval::cos(double number)
{
    return std::cos(number);
}

double DGEval::tan(double number)
{
    return std::tan(number);
}

double DGEval::pi()
{
    return PI;
}

double DGEval::atan(double number)
{
    return std::atan(number);
}

double DGEval::asin(double number)
{
    return std::asin(number);
}

double DGEval::acos(double number)
{
    return std::acos(number);
}

double DGEval::exp(double number)
{
    return std::exp(number);
}

double DGEval::ln(double number)
{
    return std::log(number);
}

double DGEval::print(string *str)
{
    cout << *str << endl;
    return 0;
}

double DGEval::random(double number)
{
    return std::fmod(std::rand(), number);
}

double DGEval::len(string *str)
{
    return str->size();
}

string *DGEval::right(DGEval *dgEval, string *str, double n)
{
    int strSize = str->size();
    return  new std::string(str->substr(strSize - n));
}

string *DGEval::left(DGEval *dgEval, string *str, double n)
{
    return  new std::string(str->substr(0, n));
}

// Library End

void X64CodeBag::backpatchInstruction(DGEvalICInst *inst)
{
    switch(inst->opCode)
    {
        case JMP:
            {
                int relativeJumpOffset = dgEval->ic->instructionAt(inst->p1)->codeOffset - inst->codeOffset - JMP_LEN;

                char bytes[4];
                for(int i = 0; i < 4 ; i++) // little endian conversion
                    bytes[i] = (relativeJumpOffset >> ( i * 8)) & 0xFF;

                int addressLocationStart = inst->codeOffset + 1;
                for(int i = 0; i <  4; i++)
                    codeBase[i + addressLocationStart] = bytes[i];
            }
            break;
        case JT: // TO CHECK (should JT and JF be exactly the same ?)
        case JF:
            {   
                int     JEexactLoc = inst->codeOffset + 4, // Beforehand instructions take 4 bytes
                        relativeJumpOffset = dgEval->ic->instructionAt(inst->p1)->codeOffset - JEexactLoc - JE_LEN;

                char bytes[4];
                for(int i = 0; i < 4 ; i++) // little endian conversion
                    bytes[i] = (relativeJumpOffset >> ( i * 8)) & 0xFF;

                int addressLocationStart = inst->codeOffset + 6; // We add 6 due to some previous instructions added beforehand of JE
                for(int i = 0; i <  4; i++)
                    codeBase[i + addressLocationStart] = bytes[i];
            }
            break;
        default:
            break;
    }
}

void X64CodeBag::translateInstructionStudent(DGEvalICInst *inst)
{   
    inst->codeOffset = codeLen; // for jump

    switch(inst->opCode)
    {
        case CONST:
            if(inst->type.type == DGEvalType::DGNumber)
            {
                this->emitBytes(2, 0x48, 0xB8);                 // 00000025 48B8000000000000F8-        mov rax, __float64__(1.5)
                this->emitCodeFrag((double)inst->numConstant);  // 0000002E 3F 
                this->emitBytes(1, 0x50);                       // 0000002F 50                         push rax
            }

            if(inst->type.type == DGEvalType::DGBoolean)
            {   
                if(inst->boolConstant)
                    this->emitBytes(2, 0x6a, 0x01);         // 00000030 6A02                       push 2h
                else 
                    this->emitBytes(2, 0x6a, 0x00);         // 00000030 6A02                       push 2h
            }

            break;
        case OP_ASSIGN:
            this->emitBytes(4, 0x48, 0x8b, 0x04, 0x24);     // 000000D0 488B0424                   mov rax, [rsp]
            this->emitBytes(3, 0x48, 0x89, 0x85);           // 000000D4 48898500020000             mov [rbp+512], rax
            this->emitCodeFrag(-8 * (inst->p1+1));
            break;
        case POP:
            this->emitBytes(3, 0x48, 0x81, 0xc4);           // 000000DB 4881C408020000             add rsp, 520
            this->emitCodeFrag(8*inst->p1);
            break;
        case INSID:
            this->emitBytes(2, 0xff, 0xb5);                 // 0000005D FFB500020000               push qword [rbp + 512]
            this->emitCodeFrag(-8 * (inst->p1+1));          // 00000063 FFB508020000               push qword [rbp + 520]
            break;
        case OP_MUL:
            this->xmmArithInstruction(0x59);
            break;
        case OP_DIV:
            this->xmmArithInstruction(0x5e);
            break;
        case OP_SUB:
            this->xmmArithInstruction(0x5c);
            break;
        case OP_ADD:
            this->xmmArithInstruction(0x58);
            break;
        case OP_MINUS:
            this->emitBytes(9, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);   // 00000069 48B800000000000000-        mov rax, 0x8000000000000000
            this->emitBytes(1, 0x80);                                                   // 00000072 80
            this->emitBytes(4, 0x48, 0x31, 0x04, 0x24);                                 // 00000073 48310424                   xor [rsp], rax
            this->emitBytes(5, 0xf2, 0x0f, 0x10, 0x04, 0x24);                           // 00000077 F20F100424                 movsd xmm0, [rsp]
            break;
        case OP_NOT:
            this->emitBytes(5, 0xb8, 0x01, 0x00, 0x00, 0x00);       // 0000007C B801000000                 mov rax, 1h
            this->emitBytes(4, 0x48, 0x31, 0x04, 0x24);             // 00000081 48310424                   xor [rsp], rax
            this->emitBytes(5, 0xf2, 0x0f, 0x10, 0x04, 0x24);       // 00000085 F20F100424                 movsd xmm0, [rsp]
            break;
        case OP_BAND:
            this->emitBytes(1, 0x58);                               // 0000008A 58                         pop rax
            this->emitBytes(4, 0x48, 0x21, 0x04, 0x24);             // 0000008F 48210424                   and [rsp], rax
            break;
        case OP_BOR:
            this->emitBytes(1, 0x58);                               // 0000008A 58                         pop rax
            this->emitBytes(4, 0x48, 0x09, 0x04, 0x24);             // 0000008B 48090424                   or [rsp], rax
            break;
        case OP_GT:
            this->comparisonInstruction(inst->type, 0x7e);
            break;
        case OP_GTE:
            this->comparisonInstruction(inst->type, 0x7c); 
            break;
        case OP_LT:
            this->comparisonInstruction(inst->type, 0x7d); 
            break;
        case OP_LTE:
            this->comparisonInstruction(inst->type, 0x7f); 
            break;
        case OP_NEQ:
            this->comparisonInstruction(inst->type, 0x74);
            break;
        case OP_EQ:
            this->comparisonInstruction(inst->type, 0x75);
            break;
        case JMP:
            this->emitBytes(5, 0xe9, 0x00, 0x00, 0x00, 0x00);
            break;
        case JF:
            this->emitBytes(1, 0x58);                               // 000001EE 58                         pop rax
            this->emitBytes(3, 0x48, 0x09, 0xc0);                   // 000001EF 4809C0                     or rax, rax
            this->emitBytes(6, 0x0f, 0x84, 0x00, 0x00, 0x00, 0x00); // 0:  0f 84 00 00 00 00       je     0x6 
            break;
        case JT:
            this->emitBytes(1, 0x58);                               // 000001EE 58                         pop rax
            this->emitBytes(3, 0x48, 0x09, 0xc0);                   // 000001EF 4809C0                     or rax, rax
            this->emitBytes(6, 0x0f, 0x85, 0x00, 0x00, 0x00, 0x00); // 0:  0f 85 00 00 00 00       jne    0x6 
            break;
        default:
            this->translateInstruction(inst);
            break;
    }
}

// Generates x64 executable function using the IC. This is the bonus part.
DGEvalDynamicFunc *DGEval::generateCode()
{
    X64CodeBag *codeBag = new X64CodeBag(this);

    int symbolCount = this->symbolTable.symbolCount(),
        instSize = this->ic->instCount();

    codeBag->emitPrologue(symbolCount);

    for(int i = 0; i < instSize ; i++)
    {
        DGEvalICInst *inst = this->ic->instructionAt(i);
        codeBag->translateInstructionStudent(inst);
    }

    codeBag->emitEpilogue();

    // backpatching
    for(int i = 0; i < instSize ; i++)
    {
        DGEvalICInst *inst = this->ic->instructionAt(i);

        if(inst->opCode == JMP || inst->opCode == JT || inst->opCode == JF)
            codeBag->backpatchInstruction(inst);
    }

    return (DGEvalDynamicFunc *)codeBag->createCodeBase();
}

// Above part was related to x64 code generation. Below is for non-bonus

// Scans the expression nodes to perform constant folding in expressions.
void DGEval::scanConstantFolding()
{
    DGEvalStatementNode *head=this->draftedStatements->head;

    while(head)
    {
        scanConstantFolding(nullptr, head->exp);
        head=head->next;
    }
}

// Scans the expression nodes to perform constant folding in expressions.
void DGEval::scanConstantFolding(DGEvalExpNode *parentNode, DGEvalExpNode *node)
{

    if(node->left)
        scanConstantFolding(node, node->left);

    if(node->right)
        scanConstantFolding(node, node->right);

    switch(node->opCode)
    {   
        case OP_COMMA:
            if(node->left && node->right) // calculate the stack load for each nped
            {
                node->stackLoad = node->left->stackLoad + 1;
            }
            
            if( (optimization & OPTIMIZE_DC_EXPPART) == 0.0 ) break;

            if(!(parentNode && parentNode->left==node && parentNode->opCode==OP_COMMA) ) // not the intermediary nodes in the expression tree
            {   
                if( !parentNode || 
                    (parentNode->opCode!=OP_CALL && !(parentNode->opCode==LRT && parentNode->idNdx==0.0)))
                {
                    vector<DGEvalExpNode *> *vec = expressionPartVector(node);

                    int size = vec->size()-2;

                    for(int i = size; i >= 0; i--) // set eliminateIC for those to be eliminated
                    {
                        DGEvalExpNode *cur = (*vec)[i];

                        if(cur->assignmentCount == 0.0 && cur->functionCallCount == 0.0)
                            cur->eliminateIC = true;
                    }
                }
            }
            break;
        // Somewhere we should change nodes to LRT
        // For example, string and array equality check should be LRT. 
        // Is this place to do it?
        case OP_EQ:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0)
            {
                if((node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                || (node->left->type.type==DGEvalType::DGBoolean && node->right->type.type==DGEvalType::DGBoolean)) // the same logic for bool and numb
                {   
                    bool res;

                    if(node->left->type.type==DGEvalType::DGNumber)
                        res = (node->left->doubleValue) == (node->right->doubleValue);
                    else if(node->left->type.type==DGEvalType::DGBoolean)
                        res = (node->left->boolValue) == (node->right->boolValue);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            else if(node->left && node->right &&
                node->left->type.dim == 0 && node->right->type.dim == 0 && 
                node->left->type.type == DGEvalType::DGString &&  node->right->type.type == DGEvalType::DGString)
                {
                    node->opCode = LRT;
                    node->idNdx = 6;
                    node->doubleValue = 0;
                }
            else if(node->left && node->right &&
                node->left->type.dim > 0 && node->right->type.dim > 0 && 
                node->left->type ==  node->right->type)
                {
                    node->opCode = LRT;
                    node->idNdx = 7;
                    node->doubleValue = 0;
                }
            break;
        case OP_NEQ:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0)
            {
                if((node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                || (node->left->type.type==DGEvalType::DGBoolean && node->right->type.type==DGEvalType::DGBoolean)) // the same logic for bool and numb
                {
                    bool res;

                    if(node->left->type.type==DGEvalType::DGNumber)
                        res = (node->left->doubleValue) != (node->right->doubleValue);
                    else if(node->left->type.type==DGEvalType::DGBoolean)
                        res = (node->left->boolValue) != (node->right->boolValue);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            else if(node->left && node->right &&
                node->left->type.dim == 0 && node->right->type.dim == 0 && 
                node->left->type.type == DGEvalType::DGString &&  node->right->type.type == DGEvalType::DGString)
                {
                    node->opCode = LRT;
                    node->idNdx = 6;
                    node->doubleValue = 1;
                }
            else if(node->left && node->right &&
                node->left->type.dim > 0 && node->right->type.dim > 0 && 
                node->left->type.type ==  node->right->type.type)
                {
                    node->opCode = LRT;
                    node->idNdx = 7;
                    node->doubleValue = 0;
                }
            break;
        case OP_LT:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0) // cannot be applied to arr
            {
                if(node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                {
                    bool res = (node->left->doubleValue) < (node->right->doubleValue); 

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
                else if(node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
                {

                    bool res = DGEval::strcmp(node->left->stringValue, node->right->stringValue, 3);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);

                    // Which one is correct? The above one or below one? 
                    // Ask it to Erkan hoca

                    // node->opCode = LRT;
                    // node->idNdx = 6;
                    // node->doubleValue = 3;
                }
            }
            else if(node->left && node->right && 
                node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
            {
                node->opCode = LRT;
                node->idNdx = 6;
                node->doubleValue = 3;
            }
            break;
        case OP_LTE:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0) // cannot be applied to arr
            {
                if(node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                {
                    bool res = (node->left->doubleValue) <= (node->right->doubleValue); 

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
                else if(node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
                {
                    bool res = DGEval::strcmp(node->left->stringValue, node->right->stringValue, 5);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);

                    // Which one is correct? The above one or below one? 
                    // Ask it to Erkan hoca

                    // node->opCode = LRT;
                    // node->idNdx = 6;
                    // node->doubleValue = 5;
                }
            }
            else if(node->left && node->right && 
                node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
            {
                node->opCode = LRT;
                node->idNdx = 6;
                node->doubleValue = 5;
            }
            break;
        case OP_GT:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0) // cannot be applied to arr
            {
                if(node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                {
                    bool res = (node->left->doubleValue) > (node->right->doubleValue); 

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
                else if(node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
                {
                    bool res = DGEval::strcmp(node->left->stringValue, node->right->stringValue, 2);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);

                    // Which one is correct? The above one or below one? 
                    // Ask it to Erkan hoca

                    // node->opCode = LRT;
                    // node->idNdx = 6;
                    // node->doubleValue = 2;
                }
            }
            else if(node->left && node->right && 
                node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
            {
                node->opCode = LRT;
                node->idNdx = 6;
                node->doubleValue = 2;
            }
            break;
        case OP_GTE:
            if(node->left && node->right && 
                node->right->opCode==CONST && node->left->opCode==CONST &&
                node->right->type.dim==0 && node->left->type.dim==0) // cannot be applied to arr
            {
                if(node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber)
                {
                    bool res = (node->left->doubleValue) >= (node->right->doubleValue); 

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
                else if(node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
                {
                    bool res = DGEval::strcmp(node->left->stringValue, node->right->stringValue, 4);

                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->boolValue = res; 
                    node->opCode = CONST;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);

                    // Which one is correct? The above one or below one? 
                    // Ask it to Erkan hoca

                    // node->opCode = LRT;
                    // node->idNdx = 6;
                    // node->doubleValue = 4;
                }
            }
            else if(node->left && node->right && 
                node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString)
            {
                node->opCode = LRT;
                node->idNdx = 6;
                node->doubleValue = 4;
            }
            break;
        case OP_COND:
            if(node->left && node->left->opCode==CONST && 
                node->right && node->right->opCode==OP_ALT && 
                node->right->left && node->right->right)
            {
                if(node->left->boolValue
                && (node->right->right->functionCallCount==0.0 && node->right->right->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->right->right = destroyExpTree(node->right->right);
                    node->left = destroyExpTree(node->left);

                    *node=*node->right->left;
                }
                else if(!node->left->boolValue
                && (node->right->left->functionCallCount==0.0 && node->right->left->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->right->left = destroyExpTree(node->right->left);
                    node->left = destroyExpTree(node->left);

                    *node=*node->right->right;
                }
            }
            break;
        case OP_BAND:
            if(node->left && node->right 
            && node->left->opCode==CONST && node->right->opCode==CONST)
            {
                bool res = node->left->boolValue  && node->right->boolValue;
                
                node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                node->opCode = CONST;
                node->boolValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(node->left && node->left->opCode==CONST)
            {   
                if(node->left->boolValue)
                {
                    node->left = destroyExpTree(node->left);
                    *node=*node->right;
                }
                else if(!node->left->boolValue         
                && (node->right->functionCallCount==0.0 && node->right->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->opCode=CONST;
                    node->boolValue=false;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            else if(node->right && node->right->opCode==CONST)
            {
                if(node->right->boolValue)
                {
                    node->right = destroyExpTree(node->right);
                    *node=*node->left;
                }
                else if(!node->right->boolValue 
                && (node->left->functionCallCount==0.0 && node->left->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->opCode=CONST;
                    node->boolValue=false;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            break;
        case OP_BOR: 
            if(node->left && node->right 
            && node->left->opCode==CONST && node->right->opCode==CONST)
            {
                bool res = node->left->boolValue || node->right->boolValue;
                
                node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                node->opCode = CONST;
                node->boolValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(node->left && node->left->opCode==CONST)
            {   
                if(!node->left->boolValue)
                {
                    node->left = destroyExpTree(node->left);
                    *node=*node->right;
                }
                else if(node->left->boolValue
                && (node->right->functionCallCount==0.0 && node->right->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->opCode = CONST;
                    node->boolValue = true;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            else if(node->right && node->right->opCode==CONST)
            {
                if(!node->right->boolValue)
                {
                    node->right = destroyExpTree(node->right);
                    *node=*node->left;
                }
                else if(node->right->boolValue 
                && (node->left->functionCallCount==0.0 && node->left->assignmentCount==0.0)) // if check'e ass count ve func count ekle
                {
                    node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                    node->opCode = CONST;
                    node->boolValue = true;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            }
            break;
        case OP_ADD:
            if(node->left && node->right && node->left->opCode==CONST && node->right->opCode==CONST
            && node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGNumber 
            && node->left->type.dim==0 && node->right->type.dim==0)
            {
                // Both operands are number constants
                double res = node->left->doubleValue + node->right->doubleValue;

                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);

            }
            else if(node->left && node->right && node->left->opCode==CONST && node->right->opCode==CONST
                && node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGString 
                && node->left->type.dim==0 && node->right->type.dim==0)
                {
                    // Both operands are string constants
                    node->type.type = DGEvalType::DGString; node->type.dim = 0;
                    node->opCode = CONST;

                    node->stringValue = new std::string();
                    *node->stringValue = (*node->left->stringValue) + (*node->right->stringValue);

                    node->left->stringValue = node->right->stringValue = nullptr;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                }
            else if(node->left && node->right && node->left->opCode==CONST && node->right->opCode==CONST
                && node->left->type.type==DGEvalType::DGString && node->right->type.type==DGEvalType::DGNumber
                && node->left->type.dim==0 && node->right->type.dim==0)
                {
                    // Left is string operand, right is numb operand
                    node->type.type = DGEvalType::DGString; node->type.dim = 0;
                    node->opCode = CONST;

                    node->stringValue = new std::string();
                    *node->stringValue = (*node->left->stringValue) + (std::to_string(node->right->doubleValue));

                    node->left->stringValue = node->right->stringValue = nullptr;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                } 
            else if(node->left && node->right && node->left->opCode==CONST && node->right->opCode==CONST
                && node->left->type.type==DGEvalType::DGNumber && node->right->type.type==DGEvalType::DGString
                && node->left->type.dim==0 && node->right->type.dim==0)
                {
                    // left is number operand, right is string operand
                    node->type.type = DGEvalType::DGString; node->type.dim = 0;
                    node->opCode = CONST;

                    node->stringValue = new std::string();
                    *node->stringValue =  (std::to_string(node->left->doubleValue)) + (*node->right->stringValue);

                    node->left->stringValue = node->right->stringValue = nullptr;
                    node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
                } 
            else if(
                (node->right && node->right->opCode==CONST &&  node->right->type.type==DGEvalType::DGNumber  && node->right->doubleValue==0.0 
                && node->left && node->left->type.type==DGEvalType::DGNumber
                && node->left->type.dim==0 && node->right->type.dim==0)
                || 
                (node->right && node->right->opCode==CONST && node->right->type.type==DGEvalType::DGString && *node->right->stringValue==""
                && node->left && node->left->type.type==DGEvalType::DGString
                && node->left->type.dim==0 && node->right->type.dim==0))
            {
                // if both operands are number and right has 0.0 value OR if both operands are string and right has "" (empty string) value
                node->right = destroyExpTree(node->right);
                *node=*node->left;
            }
            else if(
                (node->left && node->left->opCode==CONST && node->left->type.type==DGEvalType::DGNumber && node->left->doubleValue==0.0 
                && node->right && node->right->type.type==DGEvalType::DGNumber
                && node->left->type.dim==0 && node->right->type.dim==0)
                ||
                (node->left && node->left->opCode==CONST && node->left->type.type==DGEvalType::DGString && *node->left->stringValue==""
                && node->right && node->right->type.type==DGEvalType::DGString 
                && node->left->type.dim==0 && node->right->type.dim==0))
            {
                // if both operands are number and left has 0.0 value OR if both operands are string and right has "" (empty string) value
                node->left = destroyExpTree(node->left);
                *node=*node->right;
            }
            else if(node->left && node->left->type.type==DGEvalType::DGString && node->left->type.dim==0 &&
                node->right && node->right->type.type==DGEvalType::DGString && node->right->type.dim==0 &&
                (node->left->opCode != CONST ||  node->right->opCode != CONST ))
            {
                // string concat case
                node->opCode = LRT;
                node->idNdx = 4;

            }
            else if(node->left && node->right &&
                node->left->type.type == node->right->type.type && node->left->type.dim == node->right->type.dim + 1)
            {
                // TO DO and TO CHECK. I Think this is all. Ask it to Erkan hoca
                // Array append case

                node->opCode = LRT;
                node->idNdx = 2;
            }
            break;
        case OP_SUB:
            if(node->left && node->right
            && node->left->opCode==CONST && node->right->opCode==CONST)
            {
                double res = node->left->doubleValue - node->right->doubleValue;

                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(node->left && node->left->opCode==CONST && node->left->doubleValue==0.0)
            {    
                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = OP_MINUS;
                node->left = destroyExpTree(node->left);
                node->left = node->right; node->right = nullptr;
            }
            else if(node->right && node->right->opCode==CONST && node->right->doubleValue==0.0)
            {
                node->right = destroyExpTree(node->right);
                *node = *node->left;
            }
            break;
        case OP_MUL:
            if(node->left && node->right
            && node->left->opCode==CONST && node->right->opCode==CONST)
            {
                double res = node->left->doubleValue * node->right->doubleValue;

                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(
               (node->left && node->left->opCode==CONST && node->left->doubleValue==0.0 
               && (node->right && node->right->functionCallCount==0.0 && node->right->assignmentCount==0.0))  // if check'e ass count ve func count ekle
            || (node->right && node->right->opCode==CONST && node->right->doubleValue==0.0
                && (node->left && node->left->functionCallCount==0.0 && node->left->assignmentCount==0.0)))
            {       
                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = 0.0;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(node->left && node->left->opCode == CONST && node->left->doubleValue == 1.0)
            {
                node->left = destroyExpTree(node->left);
                *node = *node->right;
            }
            else if(node->right && node->right->opCode == CONST && node->right->doubleValue == 1.0)
            {
                node->right = destroyExpTree(node->right);
                *node = *node->left;
            }
            break;
        case OP_DIV:
            if( node->right  && node->right->opCode==CONST && node->right->doubleValue==0.0)
            {
                messageSet.appendMessage(node->lineNumber, "Division By 0!", DGEvalMsgSeverity::Error);
                
                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = 3.0;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            else if(
              ( (node->left && node->left->opCode==CONST && node->left->doubleValue==0.0) 
                && (node->right && node->right->functionCallCount==0.0 && node->right->assignmentCount==0.0) ) // if check'e ass count ve func count ekle, burası değişicek gibi
            || (node->right && node->right->opCode==CONST && node->right->doubleValue==1.0))
            {
                node->right = destroyExpTree(node->right);
                *node=*node->left;
            }
            else if(node->right && node->right->opCode==CONST
            && node->left && node->left->opCode==CONST)
            {
                double res = node->left->doubleValue / node->right->doubleValue;

                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = res;
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            break;
        case OP_MINUS:
            if(node->left && node->left->opCode==CONST)
            {   
                node->type.type = DGEvalType::DGNumber; node->type.dim = 0;
                node->opCode = CONST;
                node->doubleValue = -(node->left->doubleValue);
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            break;
        case OP_NOT:
            if(node->left && node->left->opCode==CONST)
            {
                node->type.type = DGEvalType::DGBoolean; node->type.dim = 0;
                node->opCode = CONST;
                node->boolValue = !(node->left->boolValue);
                node->left = destroyExpTree(node->left); node->right = destroyExpTree(node->right);
            }
            break;
        case OP_AA:
            node->opCode = LRT;
            node->idNdx = 1;
            break;
        default:
            break;
    }

    transformStrConstToLRT(node->left); transformStrConstToLRT(node->right);
    if(!parentNode) transformStrConstToLRT(node);

}

// Scans the drafted statements to generate intermediate code.
void DGEval::scanForIC()
{
    // This part may be the next part to go on after constant folding

    DGEvalStatementNode *head=this->draftedStatements->head;

    this->ic = new DGEvalIC(); 

    while(head)
    {
        if( (optimization & OPTIMIZE_DC_STATEMENT) == 0 // no optimization   
            ||  ((optimization & 0x01) != 0) && (head->exp->assignmentCount!=0 || head->exp->functionCallCount!=0 || head->asPredecessorCount!=0)) // death statement eliminatio
        {
            scanForIC(nullptr, head->exp);
            DGEvalICInst *inst = ic->emitIC(POP, 1);
        }
        head=head->next;
    }

    ic->emitIC(LRT, 8); // Call for clean-up
}

// Scans the expression nodes to generate intermediate code.
void DGEval::scanForIC(DGEvalExpNode *parentNode, DGEvalExpNode *node)
{
    if(node->eliminateIC) return;

    if(node->left)
        scanForIC(node, node->left);

    int jmpInstrLHScondIndx = -1;

    switch(node->opCode)
    {   
        case OP_COND:
            if(ic->emitIC(JF, 0, node->type)  )// 0 is temporary  
            {
                jmpInstrLHScondIndx = ic->instCount()-1;           
            }
            break;
        case OP_ALT:
            {   
                ic->emitIC(JMP, 0, node->type); // 0 is temporary
                int icSize = ic->instCount();
                node->altTrueEnd = --icSize; // this is the index of jmp instruction
            }
            break;
        default:
            break;
    }

    if(node->right)
        scanForIC(node, node->right);

    switch(node->opCode)
    {      
        case LRT:
            if(DGEvalICInst *inst = ic->emitIC(node->opCode, node->idNdx, node->type))
            {

                switch(node->idNdx)
                {   
                    case 0:
                        inst->numConstant = node->pVector->size();
                        break;
                    case 3:
                        inst->strConstant = new std::string(*node->stringValue); 
                        break;
                    case 4: 
                        break;
                    case 6:
                        inst->numConstant = node->doubleValue;
                        break;
                    default:
                        // TO CHECK
                        //inst->numConstant = node->doubleValue;
                        break;
                }
            }
            break;
        case OP_COMMA:
            if(!(parentNode && parentNode->left==node && parentNode->opCode==OP_COMMA) ) // not the intermediary nodes in the expression tree
            {   
                if( !parentNode || 
                    (parentNode->opCode!=OP_CALL && !(parentNode->opCode==LRT && parentNode->idNdx==0.0)))
                {
                    int numbOfEliminatedNodes = 0;

                    vector<DGEvalExpNode *> *vec = expressionPartVector(node);

                    for(DGEvalExpNode *cur: *vec) // set eliminateIC for those to be eliminated
                    {
                        if(cur->eliminateIC)
                            numbOfEliminatedNodes++;
                    }

                    int popAmount = node->stackLoad - numbOfEliminatedNodes - 1;

                    if(popAmount > 0)
                        DGEvalICInst *inst = ic->emitIC(POP,  popAmount);
                }
            }
            break;
        case OP_COND:
            if(jmpInstrLHScondIndx!=-1 && node->right && node->right->opCode==OP_ALT)
            {
                DGEvalICInst *ins = ic->instructionAt(jmpInstrLHScondIndx);
                ins->p1 = node->right->altTrueEnd+1;
            }
            break;
        case OP_ALT:
            {
                int icSize = ic->instCount();
                node->altFalseEnd = icSize; // this is the index of the next instruction after false case

                DGEvalICInst *altTrueEndJmpInst = ic->instructionAt(node->altTrueEnd);
                altTrueEndJmpInst->p1 = node->altFalseEnd;
            }
            break;
        case OP_ASSIGN:
            if(node->left && node->left->opCode==INSID && node->left->stringValue)
            {
                DGEvalSymbolDesc  *symbol = symbolTable.findSymbol(*node->left->stringValue);

                if(symbol)
                {
                    DGEvalICInst *inst = ic->emitIC(node->opCode, symbol->ndx, symbol->type);

                    if(inst)
                    {
                        inst->p1 = symbol->ndx;
                        inst->strConstant = new std::string(*node->left->stringValue);
                    }
                }
            }
            break;
        case OP_BAND:
        case OP_BOR:
        case OP_EQ:
        case OP_NEQ:
        case OP_LT:
        case OP_LTE:
        case OP_GT:
        case OP_GTE:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_ADD:
        case OP_MINUS:
        case OP_NOT:
            ic->emitIC(node->opCode, 0, node->left->type); // left and right has the same type of operands
            break;
        case INSID:
            if(parentNode && !(parentNode->left==node && parentNode->opCode==OP_ASSIGN))
            {   // need curly braces to define new variables
                DGEvalSymbolDesc  *symbol = symbolTable.findSymbol(*node->stringValue);

                if(symbol) // we have found the symbol
                {   
                    // push instruction
                    DGEvalICInst *inst = ic->emitIC(node->opCode, symbol->ndx, symbol->type);

                    if(inst)
                        inst->strConstant = new std::string(*node->stringValue);
                }
            }
            break;
        case CONST:
            if(DGEvalICInst *inst = ic->emitIC(node->opCode, 0, node->type))
            {
                if(node->type.type == DGEvalType::DGBoolean)
                    inst->boolConstant = node->boolValue;
                else if(node->type.type == DGEvalType::DGNumber)
                    inst->numConstant = node->doubleValue;
                else if(node->type.type == DGEvalType::DGString)
                    inst->strConstant = new std::string(*node->stringValue);
            }
            break;
        case OP_CALL:
            if(node->left->opCode==INSID && node->left->stringValue)
            {   
                for(int i = 0; i < NUMB_OF_FUNCS; i++)
                {
                    DGEvalFuncDesc *func =  DGEval::libFunctionAt(i);

                    if(func->name == (*node->left->stringValue))
                    {
                        DGEvalICInst* lastInst = ic->emitIC(node->opCode, node->pVector->size(), node->type);
                        lastInst->numConstant = (int)i;
                        break;
                    }
                }
            }
            break;
        default:
            break;
    }
}

// Scans the linear IR
void DGEval::peepholeIC()
{
    if(((optimization & OPTIMIZE_PH_OFFLOAD) == 0)
    && ((optimization & OPTIMIZE_PH_CONSTSINK) == 0 )) 
        return; // no optimization

    bool    change = true,
            optimizeOffLoad = false,
            optimizeConstSink = false;

    // applyRemoval and mark removal functions can be used here
    while(change) 
    {
        change = optimizeOffLoad = optimizeConstSink = false;

        int size =  ic->instCount();

        for(int i=2; i<size; i++)
        {
            DGEvalCodePathWindow *cdp = new DGEvalCodePathWindow ();
            
            cdp->build(this->ic, i, WINDOW_SIZE);

            int numbOfPaths = cdp->path.size();

            if(numbOfPaths > 0)
            {
                DGEvalICInst    *FirstPathInsts[3]= {   
                                                        this->ic->instructionAt((*cdp->path[0])[0]),
                                                        this->ic->instructionAt((*cdp->path[0])[1]),
                                                        this->ic->instructionAt((*cdp->path[0])[2])
                                                    };

                if(((optimization & OPTIMIZE_PH_OFFLOAD) != 0) 
                && FirstPathInsts[0]->opCode==OP_ASSIGN
                && FirstPathInsts[1]->opCode==POP
                && FirstPathInsts[2]->opCode==INSID
                && *FirstPathInsts[0]->strConstant==*FirstPathInsts[2]->strConstant)
                    optimizeOffLoad = true;

                if(((optimization & OPTIMIZE_PH_CONSTSINK) != 0)
                && (FirstPathInsts[0]->opCode==CONST || (FirstPathInsts[0]->opCode==LRT && FirstPathInsts[0]->p1==3))
                && FirstPathInsts[1]->opCode==POP)
                    optimizeConstSink = true;

                
                for(int j=1; j<numbOfPaths; j++)
                {
                    DGEvalICInst    *tempPathInsts[3]=  {   
                                                    this->ic->instructionAt((*cdp->path[j])[0]),
                                                    this->ic->instructionAt((*cdp->path[j])[1]),
                                                    this->ic->instructionAt((*cdp->path[j])[2])
                                                };

                    if(((optimization & OPTIMIZE_PH_OFFLOAD) != 0) 
                    && 
                        !(tempPathInsts[0]->opCode==FirstPathInsts[0]->opCode
                        && tempPathInsts[1]->opCode==FirstPathInsts[1]->opCode
                        && tempPathInsts[2]->opCode==FirstPathInsts[2]->opCode
                        && *tempPathInsts[0]->strConstant==*tempPathInsts[2]->strConstant 
                        && *tempPathInsts[0]->strConstant==*FirstPathInsts[0]->strConstant))
                        optimizeOffLoad = false;

                    if(((optimization & OPTIMIZE_PH_CONSTSINK) != 0)
                    &&
                        !(tempPathInsts[1]->opCode==FirstPathInsts[1]->opCode
                        && (tempPathInsts[0]->opCode==CONST || (tempPathInsts[0]->opCode==LRT && tempPathInsts[0]->p1==3))
                        )
                    )
                        optimizeConstSink = false;
                }

                // Delete if optimization  flags have been set
                if(numbOfPaths>0 && (optimizeOffLoad || optimizeConstSink))
                {
                    for(int j=0; j<numbOfPaths;j++)
                    {
                        int firstInstInd = (*cdp->path[j])[0];
                        
                        if(optimizeOffLoad)
                            this->ic->markRemoval(firstInstInd+1, 2);
                        else if(optimizeConstSink)
                            this->ic->markRemoval(firstInstInd, 2);
                    }

                    this->ic->applyRemoval();
                    change = true;
                    break;
                }
            }
        }
    }
}

// New WriteAsJsons
void DGEvalIC::writeAsJSON(ostream *outStream)
{
    int size = code.size()-1;

    (*outStream) << "[";
    for(int i = 0; i < size ; i++)
    {
        DGEvalCodeLocation cl = code[i];

        cl.inst.writeAsJSON(outStream);
        (*outStream) << ",";
    }

    code[size].inst.writeAsJSON(outStream);
    (*outStream) << "]";
}

void DGEvalICInst::writeAsJSON(ostream *outStream)
{
    (*outStream)    << "{"
                    << "\"mnemonic\":\"" << DGEval::opStr[opCode] << "\","
                    << "\"opCode\":" << opCode <<  ","
                    << "\"p1\":" << p1 << ","
                    << "\"type\":" << (int)type.type << ","
                    << "\"dim\":" << type.dim;

    switch(opCode)
    {
        case CONST:
            if(type.dim == 0)
            {
                if(type.type==DGEvalType::DGString)
                {
                    (*outStream) << ",\"value\":"; 
                    DGEval::writeStrValue(outStream, strConstant); 
                }
                else if(type.type==DGEvalType::DGNumber)
                    (*outStream) << ",\"value\":" << numConstant;
                else if(type.type==DGEvalType::DGBoolean)
                    (*outStream) << ",\"value\":" << boolConstant;
            }   
            break;
        case INSID:
            (*outStream) << ",\"id\":"; DGEval::writeStrValue(outStream, strConstant); 
            break;
        case OP_CALL:
            (*outStream) << ",\"name\":"; DGEval::writeStrValue(outStream, &DGEval::libFunctionAt((int)numConstant)->name);
            (*outStream) << ",\"id\":" << numConstant; 
            break;
        case LRT:
            if(p1 == 3)
            {
                (*outStream) << ",\"value\":"; DGEval::writeStrValue(outStream, strConstant);
            }
            else 
                (*outStream) << ",\"value\":" << numConstant;
            break;
        default: break;
    }
    (*outStream)    << "}";
}