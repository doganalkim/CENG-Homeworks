using namespace std;

#include <bits/stdc++.h>
#include <string>
#include <vector>
#include "dgevalsyn.tab.hh"
#include <iostream>
#include <fstream>
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"
#include "dgevalsup.h"

// This is my special function
void MyParser::incrementLine()
{
   curLine++;
}


// FUNCTIONS TO BE USED IN THE BISON PART

DGEvalExpNode *MyParser::processNumericConstant(double value)
{
    return new DGEvalExpNode(curLine, value, CONST);
}

DGEvalExpNode *MyParser::processBooleanConstant(bool value)
{
    return new DGEvalExpNode(curLine, value, CONST);
}

DGEvalExpNode *MyParser::processStringConstant(string *str)
{
    return new DGEvalExpNode(curLine, str, CONST);
}

DGEvalExpNode *MyParser::processID(string *id)
{
    return new DGEvalExpNode(curLine, id, INSID);
}

DGEvalExpNode *MyParser::processArrayLiteral(DGEvalExpNode *expression)
{
    // TO CHECK
    DGEvalExpNode *newNode = new  DGEvalExpNode(curLine, expression); 
    //newNode->type.dim = expression->type.dim + 1; // DO THIS IN THE SEMANTIC CHECK PART
    return newNode;
}

DGEvalIdentifierList *MyParser::createIdentifierList(string *id)
{
    return new DGEvalIdentifierList(id);
}

DGEvalIdentifierList *MyParser::appendToIdentifierList(DGEvalIdentifierList *identifierList, string *id)
{
    identifierList->append(id);
    return identifierList;
}

DGEvalExpNode *MyParser::processUnaryOperand(DGEvalExpNode *baseOperand, DGEvalExpNode *postfixOperand)
{
    // TO CHECK 
    postfixOperand->left = baseOperand;
    return postfixOperand;
}

DGEvalExpNode *MyParser::createPostfixNode(int pOpCode, DGEvalExpNode *operand)
{
    // TO CHECK
    return new DGEvalExpNode(curLine, nullptr, pOpCode, operand);
}

DGEvalExpNode *MyParser::processBinaryOperand(int pOpCode, DGEvalExpNode *operand)
{
    // We somehow need to handle linenumber / HANDLE ERROR CONDITION
    return new DGEvalExpNode(curLine, operand, pOpCode);
}

DGEvalExpNode *MyParser::processBILRNode(DGEvalExpNode *baseNode, int opCode, DGEvalExpNode *newOperand)
{   
    return new DGEvalExpNode(curLine, baseNode, opCode, newOperand);
}

DGEvalExpNode *MyParser::processBIRLNode(DGEvalExpNode *baseNode, int opCode, DGEvalExpNode *newOperand)
{
    DGEvalExpNode *newNode = nullptr;
    if(!baseNode->left && !baseNode->right) // We have detected a leaf 
    {
        // This is the base case and the same as BILR
        return new DGEvalExpNode(curLine, baseNode, opCode, newOperand);
    }
    else
    {
        DGEvalExpNode *rightMostChild = baseNode, *parentOfRightMostChild = rightMostChild;
        while(rightMostChild->right)
        {
            parentOfRightMostChild = rightMostChild;
            rightMostChild = rightMostChild->right;
        }

        // Now, rightMostChild is the rightmost leaf of the given baseTree
        // And, parentOfRightMostChild is the parent of it.
        newNode = new DGEvalExpNode(curLine, rightMostChild, opCode, newOperand);
        parentOfRightMostChild->right = newNode;
        
        return baseNode;
    }

    return nullptr;
}  

DGEvalExpNode *MyParser::processAssignmentSide(DGEvalExpNode *conditionNode, DGEvalExpNode *alternatives)
{
    // TO CHECK / CHECKED AND SEEMS TO BE CORRECT
    if(alternatives)
    {
        return  new DGEvalExpNode(curLine, conditionNode, OP_COND, alternatives);
    }

    return conditionNode;
}

DGEvalWaitStatement *MyParser::createWaitStatement(DGEvalExpNode *expression, DGEvalIdentifierList *identifierList)
{
    // TO CHECK 
    return  new DGEvalWaitStatement(curLine, expression, identifierList);
}


DGEvalExpStatement *MyParser::createExpStatement(DGEvalExpNode *expression)
{
    return new DGEvalExpStatement(curLine, expression);
}

DGEvalStatementList *MyParser::createStatementList(DGEvalStatementNode *statement)
{ 
    return  new DGEvalStatementList(statement);
}

DGEvalStatementList *MyParser::appendToStatementList(DGEvalStatementNode *statement)
{ 
    //statementList->append(statement);
    //return statementList;

    // Fixed according to the announcement from ODTUCLASS
    dgEval->statements->append(statement);
    return dgEval->statements;
}

DGEval *MyParser::finalizeStatementList(DGEvalStatementList *statementList)
{
    this->dgEval->statements = statementList;
    return this->dgEval;
}



// WRITE JSON FUNCTIONS

void traverseDGEvalStatementNode(ostream *os, DGEvalExpNode *node)
{      
    node->writeAsJSON(os);

    if(node -> left)
    {
        (*os) << ",\"left\":{";
        traverseDGEvalStatementNode(os, node->left);
        (*os) << "}";
    }

    if(node -> right)
    {
        (*os) << ",\"right\":{";
        traverseDGEvalStatementNode(os, node->right);
        (*os) << "}";
    }
}

void DGEval::writeAsJSON(ostream *os)
{   
    // TO CHECK

    // DGEvalStatementList *statements

    (*os) << "{";

    (*os) << "\"circularStatements\":[";
    statements->writeAsJSON(os);
    (*os) << "],";

    symbolTable.writeAsJSON(os);

    (*os) << "\"executablestatements\":[";
    draftedStatements->writeAsJSON(os);
    (*os) << "]";

    (*os) << ",\"messages\":[";
    messageSet.writeAsJSON(os);
    (*os) << "]";

    (*os) << "}";
}

// Generates a JSON value, which is array of the messages contained.
void DGEvalMsgSet::writeAsJSON(ostream *os)
{
    int size = messages.size();

    for(int i = 0 ; i < size ; i++)
    {
        DGEvalMsgDesc *cur = messages[i];

        (*os) << "\"";

        if(cur->lineNumber != INT_MAX && cur->lineNumber != INT_MIN) // Comments say these values are used not to print lineNumber
        {
            (*os) << "Line number " << cur->lineNumber  << " ";
        }
        
        switch(cur->s)
        {
            case DGEvalMsgSeverity::Error:
                (*os) << "[Error]: "; 
                break;
            case DGEvalMsgSeverity::Warning:
                (*os) << "[Warning]: "; 
                break;
            case DGEvalMsgSeverity::Info:
                (*os) << "[Info]: "; 
                break;
            default:
                break;
        }

        (*os) << cur->msg << "\"";

        if(i != size-1)
            (*os) << ",";
    }
}

void DGEvalExpNode::writeAsJSON(ostream *os)
{   
    // TO DO
    (*os) << "\"lineNumber\":" << lineNumber;
    (*os) << ",\"nodeType\":" << "\"expression node\"";
    (*os) << ",\"opCode\":" << opCode;
    (*os) << ",\"mnemonic\":" << "\"" <<  DGEval::opStr[opCode] << "\"";

    string newType;

    switch(type.type)
    {
        case DGEvalType::DGNumber:
            newType = "number";
            break;
        case DGEvalType::DGString:
            newType = "string";
            break;
        case DGEvalType::DGBoolean:
            newType = "boolean";
            break;
        case DGEvalType::DGArray:
            newType = "array";
            break;
        default:
            newType = "none";
            break;
    }

    (*os) << ",\"typeCode\": " << (int)type.type;
    (*os) << ",\"type\": \"" << newType << "\""; 
    (*os) << ",\"dim\":" << type.dim;

    if(!left && !right && opCode != INSID) // TO CHECK (SHOULD "IF" BE LIKE THIS?)
    {
        switch(type.type)
        {
            case DGEvalType::DGNumber: // NUMB
                (*os) << ",\"numberValue\":\"" << doubleValue << "\"";
                break;
            case DGEvalType::DGString: // STR
                (*os) << ",\"stringValue\":"; writeStrValue(os, stringValue);
                break;
            case DGEvalType::DGBoolean: // BOOL
                if(boolValue)
                {
                    string str = "true";
                    (*os) << ",\"booleanValue\":" << str;
                }
                else
                {
                    string str = "false";
                    (*os) << ",\"booleanValue\":" << str;
                }
                break;
            default:
                if(stringValue) // ID
                {
                    (*os) << ",\"id\":";  writeStrValue(os, stringValue); 
                }
                break;
        }
    }
    else if(opCode == INSID && stringValue)
    {
        (*os) << ",\"id\":" << "\"" << *stringValue << "\"";
    }
}

void DGEvalStatementList::writeAsJSON(ostream *os)
{   
    // DGEvalStatementNode    *head=nullptr,
    //                        *tail=nullptr;
    while(head)
    {       
        head->writeAsJSON(os);
        head = head->next;
        if(head) (*os) << ",";
    }
    return;
}

void DGEvalIdentifierList::writeAsJSON(ostream *os)
{   
    // TO DO
    int size = this->size(), i = 0;
    size--;

    for(; i < size ; i++)
    {
        string *symbol = this->identifierAt(i);
        (*os) << "\"" << *symbol  << "\",";
    }
    string *symbol = this->identifierAt(i);
    (*os) << "\"" << *symbol  << "\"";

}

void DGEvalWaitStatement::writeAsJSON(ostream *os)
{   
    // TO DO
    (*os) << "{";
    (*os) << "\"lineNumber\":" << lineNumber;
    (*os) << ",\"nodeType\":" << "\"wait statement\"";

    (*os) << ",\"expression\":{";
    
    traverseDGEvalStatementNode(os, exp);

    (*os) << "}"; // Expression ending bracket

    (*os) << ",\"idList\":[";
    identifierList->writeAsJSON(os);
    (*os) << "]";

    (*os) << "}"; // statement ending bracket

}

void DGEvalExpStatement::writeAsJSON(ostream *os)
{   
    // TO DO

    (*os) << "{";
    (*os) << "\"lineNumber\":" << lineNumber;
    (*os) << ",\"nodeType\":" << "\"expression statement\"";
    (*os) << ",\"expression\":{";
    traverseDGEvalStatementNode(os, exp);
    (*os) << "}}";
}

void DGEvalSymbolTable::writeAsJSON(ostream *os)
{
    // TO DO 
    (*os) << "\"symbols\": [";
    int size = symbols.size();

    for(int i = 0 ; i < size; i++)
    {
        DGEvalSymbolDesc *cur = symbols[i];

        (*os) << "{";

        (*os) << "\"name\": \"" << cur->name << "\"";

        switch(cur->type.type)
        {   
            case DGEvalType::DGNumber:
                (*os) << ",\"type\":" << "\"number\"" ;
                break;
            case DGEvalType::DGString:
                (*os) << ",\"type\":" << "\"string\"" ;
                break;
            case DGEvalType::DGBoolean:
            (*os) << ",\"type\":" << "\"boolean\"" ;
                break;
            default:
                (*os) << ",\"type\":" << "\"none\"" ;
                break;
        }
        (*os) << ",\"dim\":" << cur->type.dim;

        (*os) << "}";

        if(i != size-1)
        {
            (*os) << ",";
        }

    }

    (*os) << "],";

    return;
}

// DEPENDENCY FUNCTIONS

// Scans the statements to generate symbolic dependencies.
void DGEval::scanDependencies()
{
    // TO DO
    DGEvalStatementNode *ptr = statements->head;

    while(ptr)
    {   
        DGEvalExpNode *root = ptr->exp;

        this->scanDependencies(ptr, nullptr, root);

        ptr->addExplicitDependencies();

        ptr = ptr->next;
    }

    // AT THIS POINT ALL OF THE defining AND symbolDependency VARIABLES ARE SET
    // WHAT NEXT?
    // IS THIS THE PLACE FOR TOPSORT?
    // TOP SORT SHOULD BE BEFORE TYPE INFERENCE

    this->buildStatementDependencyLists(); // We have build statement dependency lists by looking at individual defining & dependency

    this->draftIndependents(); // It is time for top sort

    // Now, report circular statements as messages
    ptr = statements->head;

    while(ptr)
    {
        messageSet.appendMessage(ptr->lineNumber, "Statement in circular reference.", DGEvalMsgSeverity::Error);
        ptr = ptr->next;
    }
    

}

// Builds the statement dependency lists. Each statement has a list of statements that
// must be executed before.
void DGEval::buildStatementDependencyLists()
{
    DGEvalStatementNode *ptr = this->statements->head;

    while(ptr)
    {
        int size = ptr->symbolDependency->size();

        for(int i = 0; i < size ; i++) // For each dependency in a given statement, find defining statement
        {
            DGEvalStatementNode *head = this->statements->head;
            string *symbol = ptr->symbolDependency->identifierAt(i);

            while(head)
            {
                if(head->doesDefine(symbol)) // Now, add head to ptr's dependency list
                {   
                    // "ptr" is dependent on "head"
                    ptr->addPredecessor(head);
                }
                head = head->next;
            }
        }

        ptr = ptr -> next;
    }
}

// Finds the symbol specified by "name". Returns descriptor when found, nullptr otherwise.
DGEvalSymbolDesc *DGEvalSymbolTable::findSymbol(string name)
{
    int size = symbols.size();

    for(int i = 0 ; i < size ; i++)
    {
        if(symbols[i]->name == name)
        {
            return symbols[i];
        }
    }
    return nullptr;
}

// Inserts the symbol specified by "name". .
DGEvalSymbolDesc *DGEvalSymbolTable::addSymbol(string name, DGTypeDesc pType)
{
    DGEvalSymbolDesc *newSymbol = new DGEvalSymbolDesc(name, pType);
    symbols.push_back(newSymbol);
    return newSymbol;
}

// Performs topological sort by pruning and drafting independent statements recursively.
// Efectively, this is a transfer from the list of "statements" to the list of "draftedStatements".
void DGEval::draftIndependents()
{
    // TO DO
    bool hasChanged = false;
    DGEvalStatementNode *slow = statements->head, *fast=slow;

    while(fast)
    {   
        slow = fast;
        if(!fast->statementDependencyList)
        {
            DGEvalStatementNode *head = statements->head;

            // remove ptr from each statement dependency list
            while(head)
            {   
                if(head->removePredecessor(fast))
                {
                    cout << "SuccessFully Removed!" << endl;
                }
                head = head->next;
            }

            this->statements->remove(fast);
            this->draftedStatements->append(fast);

            hasChanged = true; // Should this line be here? Think about it once again

            break; // At this point, we have removed each outgoing edge for "fast"
        }
        fast = slow->next;
    }

    if(hasChanged) draftIndependents();
}

// Scans the the expression nodes to generate symbolic dependencies.
void DGEval::scanDependencies(DGEvalStatementNode *statement, DGEvalExpNode *parentNode, DGEvalExpNode *node)
{
    // TO CHECK
    // We should add case for handling the wait statement
    // We should add case for handling function call
    // Both of the above also contains ID

    if(node->left)
    {
        scanDependencies(statement, node, node->left);
    }

    if(node->right)
    {
        scanDependencies(statement, node, node->right);
    }

    // DEFINING
    if(node->opCode == OP_ASSIGN) // If current node is assign node
    {
        // Any id located at the left side of = operator is defined
        if(node->left->stringValue && node->left->opCode == INSID) // IF IT IS ID OR NOT
        {
            statement->addDefining(node->left->stringValue);
        }
        else // IF IT IS NOT AN ID, ERROR OR WHAT?
        {

        }
    }

    // DEPENDENCY
    if( node->stringValue && node->opCode == INSID)  
    {
        if(parentNode && !(parentNode->opCode == OP_ASSIGN && parentNode->left == node) && !(parentNode->opCode == OP_CALL && parentNode->left == node))
            statement->addDependency(node->stringValue);
        else
        {
            // HANDLE ERROR
        }
    }

}

// Scans the drafted statements to calculate types of the expressions.
void DGEval::scanCalculateTypes()
{
    DGEvalStatementNode *ptr = draftedStatements->head;

    while(ptr)
    {
        this->scanCalculateTypes(nullptr, ptr->exp);
        ptr = ptr->next;
    }

    messageSet.sort();
}

// Scans the expression nodes to calculate types of the expressions.
void DGEval::scanCalculateTypes(DGEvalExpNode *parentNode, DGEvalExpNode *node)
{
    // TO DO

    // WHAT TO DO HERE? IS THIS OUR MAIN FUNCTION FOR SEMANTIC CHECK?

    // We can reach symbolTable variable from this function

    if(node->left)
    {
        scanCalculateTypes(node, node->left);
    }

    if(node->right)
    {
        scanCalculateTypes(node, node->right);
    }

    switch(node->opCode)
    {
        case OP_ASSIGN: 
            if(node->left && node->left->opCode == INSID) // CORRECT FOR CASE 12
            {
                if(symbolTable.findSymbol(*node->left->stringValue)) // Duplicate symbols detected
                {
                    // HANDLE ERROR FOR 1
                    messageSet.appendMessage(node->lineNumber, "A variable in a DGEval program cannot be redefined.", DGEvalMsgSeverity::Error);
                }
                else // First check for error case 2 from the assignment pdf
                {
                    if(findLibFunction(*node->left->stringValue))
                    {
                        // HANDLE ERROR FOR CASE 2
                        messageSet.appendMessage(node->lineNumber, "Any runtime library function name cannot be redefined as a variable name.", DGEvalMsgSeverity::Error);
                    }
                    else
                    {
                        // CORRECT CASE
                        // NEXT TO DO
                        if(node->right)
                        {
                            node->type = node->right->type;
                            node->left->type = node->right->type; // This line is commented because id's in the output have non type
                            // insert identifier into symbol table
                            // Look how to insert it
                            symbolTable.addSymbol(*node->left->stringValue, node->type);
                        }
                    }
                }
            }
            else 
            {
                // HANDLE ERROR FOR CASE 12
                messageSet.appendMessage(node->lineNumber, "The LHS of an assignment operator must be an identifier", DGEvalMsgSeverity::Error);
            }
            break;
        case INSID:
            if(node->stringValue)
            {
                DGEvalSymbolDesc *symbol =  symbolTable.findSymbol(*node->stringValue);
                DGEvalFuncDesc *func = findLibFunction(*node->stringValue);

                if(symbol && (int)node->type.type == 0)
                {
                    node->type = symbol->type;
                }
                else if(func)
                {
                    node->type = func->types[0]; // I've set it to the return type
                }
                else
                {
                    // This is the base case
                    // It is not an error
                    // We append symbol table once we define assign node, thus no need to do something here
                }
            }
            break;
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            if(node->left && node->right)
            {
                if(node->left->type.type == DGEvalType::DGNumber && node->right->type.type == DGEvalType::DGNumber 
                && node->left->type.dim == 0 && node->right->type.dim == 0) // CORRECT CASE
                {
                    node->type =  node->left->type;
                }
                else // Error Case 
                {
                    messageSet.appendMessage(node->lineNumber, "Multiplication, subtraction, and division operators are applied to numbers only", DGEvalMsgSeverity::Error);
                }
            }
            else
            {
                // HANDLE ERROR
                messageSet.appendMessage(node->lineNumber, "Compile error! Operator inapplicability!", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_BAND:
        case OP_BOR:
            if(node->left && node->right)
            {
                if(node->left->type.type == DGEvalType::DGBoolean && node->right->type.type == DGEvalType::DGBoolean 
                    && node->left->type.dim == 0 && node->right->type.dim == 0) // CORRECT CASE
                {
                    node->type =  node->left->type;
                }
                else // Error Case 
                {
                    messageSet.appendMessage(node->lineNumber, "The boolean operators (and, or) are applied to booleans only.", DGEvalMsgSeverity::Error);
                }
            }
            else 
            {
                // HANDLE ERROR HERE
                messageSet.appendMessage(node->lineNumber, "Compile error! Operator inapplicability!", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_NOT:
            if(node->left &&  node->left->type.type == DGEvalType::DGBoolean && node->left->type.dim == 0) // CORRECT CASE
            {
                node->type = node->left->type;
            }
            else
            {
                messageSet.appendMessage(node->lineNumber, "The not (!) operator is applied on a boolean value only.", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_MINUS:
            if(node->left &&  node->left->type.type == DGEvalType::DGNumber && node->left->type.dim == 0 && !node->right)
            {
                node->type = node->left->type;
            }
            else 
            {
                // HANDLE ERROR CASE 10
                messageSet.appendMessage(node->lineNumber, "The unary minus (-) operator is applied on a numeric value only.", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_ADD:
            if(node->left && node->right  && node->left->type.dim == 0 &&  node->right->type.dim == 0 )
            {
                if(node->left->type.type == DGEvalType::DGNumber &&  node->right->type.type == DGEvalType::DGNumber)
                {
                    // CASE A FROM HOMEWORK PDF
                    node->type = node->left->type;
                }
                else if(node->left->type.type == DGEvalType::DGString &&  node->right->type.type == DGEvalType::DGString)
                {
                    // CASE B FROM HOMEWORK PDF
                    node->type = node->left->type;
                }
                else if(node->left->type.type == DGEvalType::DGNumber &&  node->right->type.type == DGEvalType::DGString)
                {
                    // CASE B FROM HOMEWORK PDF
                    node->type = node->right->type;
                }
                else if(node->left->type.type == DGEvalType::DGString &&  node->right->type.type == DGEvalType::DGNumber)
                {
                    // CASE B FROM HOMEWORK PDF
                    node->type = node->left->type;
                }
                else
                {
                    // HANDLE ERROR CASE 8
                    messageSet.appendMessage(node->lineNumber, "The operator (+) can only be used between numbers and strings.", DGEvalMsgSeverity::Error);
                }
            }
            else if(node->left && node->right && node->left->type.dim > 0 && node->left->type.type == node->right->type.type && node->left->type.dim == node->right->type.dim + 1)
            {
                node->type = node->left->type;
            }
            else 
            {
                // HANDLE ERROR CASE 8
                messageSet.appendMessage(node->lineNumber, "Wrong use of + operator", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_COND:
            if(node->left && node->left->type.type == DGEvalType::DGBoolean && node->left->type.dim == 0) // CORRECT CASE
            {
                node->type = node->right->type;
            }
            else // HANDLE ERROR 4
            {
                messageSet.appendMessage(node->lineNumber, "The first operand of a ternary operator can be a Boolean value only.", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_ALT:
            if(node->left && node->right && node->left->type == node->right->type)
            {
                node->type = node->left->type;
            }
            else // HANDLE CASE 19
            {
                messageSet.appendMessage(node->lineNumber, "Both of the types of alternation (:) operator should be of the same type ", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_LT:
        case OP_LTE:
        case OP_GT:
        case OP_GTE:
            if(node->left && node->right && node->left->type.dim == 0 && node->right->type.dim == 0 && node->left->type == node->right->type)
            {
                if(node->left->type.type == DGEvalType::DGNumber)
                {
                    node->type.type = DGEvalType::DGBoolean;
                    node->type.dim = 0;
                }
                else if(node->left->type.type == DGEvalType::DGString)
                {
                    node->type.type = DGEvalType::DGBoolean;
                    node->type.dim = 0;
                }
                else 
                {
                    messageSet.appendMessage(node->lineNumber, "In correct use of comparison operators", DGEvalMsgSeverity::Error);
                }
            }
            else
            {
                messageSet.appendMessage(node->lineNumber, "In correct use of comparison operators", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_CALL:
            if(node->left && node->left->opCode == INSID && node->left->stringValue) // LEFT SHOULD BE ID
            {
                DGEvalFuncDesc *func = findLibFunction(*node->left->stringValue);

                if(func)
                {
                    node->type = func->types[0]; // TO CHECK

                    // There exists a function with the given left hand identifier name
                    vector<DGEvalExpNode *> inputParams = expressionPartVector(node->right);

                    int inputParamCount = inputParams.size();

                    if(inputParamCount > 2 || inputParamCount != func->paraCount) // Number of parameters did not match
                    {
                        // HANDLE ERROR (NUMBER OF PARAMETERS DID NOT MATCH)
                        messageSet.appendMessage(node->lineNumber, "Formal Parameters  and given parameters did not match due to number!", DGEvalMsgSeverity::Error);
                    }
                    else // CHECK FOR EACH TYPE OF PARAMETERS
                    {
                        bool parametersAreCorrect = true;

                        for(int i = 0; i < inputParamCount ; i++)
                        {
                            DGEvalExpNode *ptr = inputParams[i];
                            //DGTypeDesc paramType;

                            // I will not remove this code. I may need it in the future
                            // if(ptr->opCode == INSID && ptr->stringValue)
                            // {
                            //     DGEvalSymbolDesc *sym = symbolTable.findSymbol(*ptr->stringValue);
                            //     if(sym)
                            //         paramType = sym->type;
                            //     else
                            //     {
                            //         paramType = ptr->type;
                            //     }
                            // }
                            // else 
                            // {
                            //     paramType = ptr->type;
                            // }


                            if( ptr->type != func->types[i+1]) // if(paramType != func->types[i+1])
                            {   
                                cout << "i:" << i <<   " funcname:" << func->name << endl;
                                cout << "ptr: type=" << (int)ptr->type.type << " dim:" << ptr->type.dim << endl;
                                cout << "func: type=" << (int)func->types[i+1].type << " dim:" << func->types[i+1].dim << endl;
                                // HANDLE ERROR MISMATCH OF PARAMETERS TYPE
                                parametersAreCorrect = false;
                                break;
                            }

                        }

                        if(parametersAreCorrect)
                        {
                            // THEN, CORRECT CASE
                            // HERE WHAT SHOULD BE DOING? Should we set the type any way regardless of the children?
                            node->type = func->types[0]; // set type to the return type
                        }
                        else 
                        {
                            //  MISMATCH OF PARAMETERS
                            messageSet.appendMessage(node->lineNumber, "Formal Parameters  and given parameters did not match due to type!", DGEvalMsgSeverity::Error);
                        }
                    }
                }
                else
                {
                    // HANDLE ERROR (NO SUCH A FUNCTION)
                    messageSet.appendMessage(node->lineNumber, "No such a function name!", DGEvalMsgSeverity::Error);
                }
            }
            else 
            {
                // HANDLE ERROR (LEFT OF THE FUNCTION CALL SHOULD BE AN IDENTIFIER) 
                // THIS IS ERROR CASE 13 FROM ASSIGNMENT PDF
                messageSet.appendMessage(node->lineNumber, "Function Name should be an identifier!", DGEvalMsgSeverity::Error);
            }
            break;
        case CONST:
            if(node->type.type == DGEvalType::DGArray)
            {
                vector<DGEvalExpNode *> arrayMembers = expressionPartVector(node->left); // Left children contains the expression

                int size = arrayMembers.size();

                bool allSameType = true;

                DGTypeDesc arrayType;


                for(int i = 1 ; i < size ; i++)
                {   
                     
                    // DGTypeDesc type1 =  arrayMembers[i-1]->type;
                    // DGTypeDesc type2 =  arrayMembers[i]->type;


                    // Are below if statements really necessary?
                    // They may not be necessary
                    // if(arrayMembers[i-1]->opCode == INSID && arrayMembers[i-1]->stringValue )
                    // {
                    //     DGEvalSymbolDesc *sym = symbolTable.findSymbol(*arrayMembers[i-1]->stringValue);
                    //     if(sym)
                    //         type1 = sym->type;
                    //     else
                    //     {
                    //         allSameType = false;
                    //         break;
                    //     }
                    // }

                    // if(arrayMembers[i]->opCode == INSID && arrayMembers[i]->stringValue )
                    // {
                    //     DGEvalSymbolDesc *sym = symbolTable.findSymbol(*arrayMembers[i]->stringValue);
                    //     if(sym)
                    //         type2 = sym->type;
                    //     else
                    //     {
                    //         allSameType = false;
                    //         break;
                    //     }
                    // }

                    if(arrayMembers[i-1]->type != arrayMembers[i]->type) // if(arrayMembers[i-1]->type != arrayMembers[i]->type)
                    {
                        allSameType = false;
                        break;
                    }
                    arrayType = arrayMembers[i]->type;
                }

                if(size > 0)
                {
                    node->type = arrayMembers[size-1]->type;
                    node->type.dim += 1;
                }

                if(allSameType)
                {
                    node->type.type = arrayType.type;
                    node->type.dim = arrayType.dim + 1;

                    if(size > 0)
                    {
                        node->type = arrayMembers[size-1]->type;
                        node->type.dim += 1;
                    }
                }
                else // different type detected 
                {
                    // HANDLE ERROR ( ARRAY CONTAINS DIFFERENT TYPES)
                    messageSet.appendMessage(node->lineNumber, "Array of different types is not allowed!", DGEvalMsgSeverity::Error);
                }
            }
            break;
        case OP_EQ:
        case OP_NEQ:
            if(node->left && node->right)
            {
                if(node->left->type != node -> right->type) // HANDLE ERROR CASE 20 FROM HOMEWORK ASSIGNMENT
                {
                    messageSet.appendMessage(node->lineNumber, "(Non)Equalivity is for the same types of two operands", DGEvalMsgSeverity::Error);
                }
                else // CORRECT CASE
                {
                    node->type.type = DGEvalType::DGBoolean;
                    node->type.dim = 0;
                }
            }
            else // HANDLE ERROR ( THERE MUST BE 2 OPERANDS)
            {
                messageSet.appendMessage(node->lineNumber, "(Non)Equalivity is for the same types of two operands", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_AA:
            if(node->left && node->right) // BOTH OF THE OPERANDS SHOULD BE THERE
            {
                // First, check if the left operand is an array
                if(node->left->type.dim == 0)
                {
                    // HANDLE ERROR (CASE 14 FROM OUR HOMEWORK ASSIGNMENT)
                    messageSet.appendMessage(node->lineNumber, "Array access operator can be applied to an array only.", DGEvalMsgSeverity::Error);
                }
                else if(node->left->opCode == INSID && node->left->stringValue && !findLibFunction(*node->left->stringValue)) 
                {   
                    // If it is an id, it may be 0 still but it may also be an array type. So, check it from symbol table
                    // Also, the id should not be an function id either
                    // Left hand side can also be a variable which had been assigned to an array
                    DGEvalSymbolDesc *sym = symbolTable.findSymbol(*node->left->stringValue);
                    if(sym && sym->type.dim != 0)
                    {   
                        vector<DGEvalExpNode *> accessExp = expressionPartVector(node->right);

                        int size = accessExp.size();
                        
                        if(size != 1)
                        {
                            // HANDLE ERROR (CASE 15 FROM OUR HOMEWORK ASSIGNMENT PDF)
                            messageSet.appendMessage(node->lineNumber, "The expression inside an array access operator may have one expression part only. Each operand of a comma operator is an expression part.", DGEvalMsgSeverity::Error);
                        }
                        else if(accessExp[0]->type.type == DGEvalType::DGNumber &&  accessExp[0]->type.dim == 0)
                        {
                            node->type.type = sym->type.type;
                            node->type.dim = sym->type.dim - 1;
                        }
                        else 
                        {
                            // HANDLE ERROR (CASE 3 FROM Our HOMEWORK PDF)
                            messageSet.appendMessage(node->lineNumber, "An index in array access operator must be a number.", DGEvalMsgSeverity::Error);
                        }
        
                    }
                    else
                    {
                        // HANDLE ERROR WRONG CASE, IT IS ID BUT IT IS NOT AN ARRAY
                        messageSet.appendMessage(node->lineNumber, "Array access operator can be applied to an array only.", DGEvalMsgSeverity::Error);
                    }
                }   
                else 
                {   // CHECK IF THE LEFT HAND SIDE IS AN CHILD WITH TYPE ARRAY
                    // get access expressions
                    vector<DGEvalExpNode *> accessExp = expressionPartVector(node->right);

                    int size = accessExp.size();

                    if(size != 1)
                    {
                        // HANDLE ERROR (CASE 15 FROM OUR HOMEWORK ASSIGNMENT PDF)
                        messageSet.appendMessage(node->lineNumber, "The expression inside an array access operator may have one expression part only. Each operand of a comma operator is an expression part.", DGEvalMsgSeverity::Error);
                    }
                    else if(accessExp[0]->type.type == DGEvalType::DGNumber &&  accessExp[0]->type.dim == 0)
                    {
                        // CORRECT CASE
                        node->type.type = node->left->type.type;
                        node->type.dim = node->left->type.dim - 1;
                    }
                    else 
                    {
                        // HANDLE ERROR (CASE 3 FROM Our HOMEWORK PDF)
                        messageSet.appendMessage(node->lineNumber, "An index in array access operator must be a number.", DGEvalMsgSeverity::Error);
                    }
                }
            }
            else 
            {
                // HANDLE ERROR CASE 
                messageSet.appendMessage(node->lineNumber, "Invalid array access!", DGEvalMsgSeverity::Error);
            }
            break;
        case OP_COMMA:
            if( node->right )
            {
                node->type = node->left->type;
            }
        case OP_NOP:
        default: 
            break;
    }
}

// Generates a vector of the expression parts separated by comma operators.
vector<DGEvalExpNode *> DGEval::expressionPartVector(DGEvalExpNode *node)
{
    // This function is used for arrays and function calls
    // We need to see the number of parameters and type of the parameters for arrays & function calls
    vector<DGEvalExpNode *> leftVector, rightVector;

    if(!node) // no parameter case should have size of 0 nodes
        return leftVector;

    if(node->opCode != OP_COMMA)
    {
        leftVector.push_back(node);
        return leftVector;
    }

    // Now, our operator is comma

    if(node->left)
    {
        leftVector = expressionPartVector(node->left);
    }

    if(node->right)
    {
        rightVector = expressionPartVector(node->right);
    }

    leftVector.insert(leftVector.end(), rightVector.begin(), rightVector.end());

    return leftVector;
}

void DGEvalWaitStatement::addExplicitDependencies()
{ 
    // We should add identifierlist to symbolDependency.
    cout << "WAIT STATEMENT!" << endl;
    symbolDependency->appendUnique(this->identifierList);
}

void DGEvalStatementNode::addExplicitDependencies()
{
    // TO DO
    // as far as I understand, we do nothing for expNodes. We should add identifierlist to symbolDependency.
    cout << "EXP STATEMENT!" << endl;
    return;
}

// Student assignment part
// Creates and places a symbol that is defined by this statement. "defining" receives the symbol entry.
// Duplicates are prevented
void DGEvalStatementNode::addDefining(string *symbol)
{
    // TO DO
    if(defining->appendUnique(symbol))
    {
        cout << *symbol << " added into defining list successfully!" << endl;
    }
    else
    {
        cout << *symbol << " could not be added into defining list due to duplicate!" << endl;
    }
}

// Creates and places a symbol that must have been calculated before execution of this statement..
// "symbolDependency" receives the symbol entry. Duplicates are prevented
void DGEvalStatementNode::addDependency(string *symbol)
{
    // TO DO
    if(symbolDependency->appendUnique(symbol))
    {
        cout << *symbol << " added into symbolDependency list successfully!" << endl;
    }
    else
    {
        cout << *symbol << " could not be added into symbolDependency list due to duplicate!" << endl;
    }
}

// Returns true if the specified "identifier" is defined by this statement.
bool DGEvalStatementNode::doesDefine(string *identifier)
{
    // TO DO
    int size = defining->size();

    for(int i = 0; i < size; i++)
    {
        if(*(defining->identifierAt(i)) == *identifier) 
            return true;
    }
    return false;
}

// Places the specified "statement" to the list of "statementDependencyList".
void DGEvalStatementNode::addPredecessor(DGEvalStatementNode *statement)
{
    DGEvalDependencyEntry *newEntry = new DGEvalDependencyEntry;
    newEntry->statement = statement;
    newEntry->next = nullptr;

    if(!statementDependencyList)
    {
        statementDependencyList = newEntry; 
        return;
    }

    DGEvalDependencyEntry *head = statementDependencyList;
    while(head->next)
    {
        head = head->next;
    }

    head->next = newEntry;
}

// Removes the specified "statement" from the list of "statementDependencyList".
bool DGEvalStatementNode::removePredecessor(DGEvalStatementNode *statement)
{
    DGEvalDependencyEntry *fast = statementDependencyList, *slow = fast;

    while(fast)
    {
        if(fast->statement == statement)
        {   
            if(statementDependencyList != fast) // If fast is not the first elemen of the linked list
                slow->next = fast->next; // We have removed the fast
            else // If fast is the first element, then we need to update statementDependencyLİst
                statementDependencyList = fast->next;
        }
        slow = fast;
        fast = fast->next;
    }

    return false;
}

// DGEval FUNCTIONS

void DGEvalIdentifierList::append(string *identifier)
{
    identifiers.push_back(identifier);
}

// Appends a statement to the list.
void DGEvalStatementList::append(DGEvalStatementNode *statement)
{
    // TO DO
    statement->prev = statement->next = nullptr;
    if(tail)
    {
        tail->next = statement;
        statement->prev = tail;
        tail = statement;
    }
    else
    {
        head = tail = statement; // We should never be here but let it stay here for just a prevention purpose
    }
}

// Removes a statement from the list.
void DGEvalStatementList::remove(DGEvalStatementNode *statement)
{
    // CODE IT AGAIN 
    if(statement == head)
    {
        head = statement->next;
    }

    if(statement == tail)
    {
        tail = statement->prev;
    }

    if(statement->next)
    {
        statement->next->prev = statement->prev;
    }

    if(statement->prev)
    {
        statement->prev->next = statement->next;
    }

    statement->next = statement->prev = nullptr;
}

bool DGEvalIdentifierList::appendUnique(string *symbol)
{
    int size = identifiers.size();

    for(int i = 0 ; i < size ; i++)
    {
        if(*identifiers[i] == *symbol)
            return false;
    }

    identifiers.push_back(symbol);

    return true;
}

bool DGEvalIdentifierList::appendUnique(DGEvalIdentifierList *otherList)
{
    // Appends the items found in the given list to the "identifiers" preventing duplicates.
    
    int size = otherList->size();

    for(int i = 0; i < size ; i++)
    {
        string *symbol = otherList->identifierAt(i);

        if(this->appendUnique(symbol))
        {
            cout << *symbol << " Successfully added!" << endl;
        }
        else 
        {
            // HANDLE ERROR
            cout << "Duplicates Prevented" << endl;
            //return false;
        }
    }

    return true;
}

// Returns the number of items in the list.
int DGEvalIdentifierList::size()
{
    return identifiers.size();
}

// Returns the identifier found with the zero based index "ndx". When not found, returns nullptr.
string *DGEvalIdentifierList::identifierAt(int ndx)
{
    int size = this->size();

    if(ndx >= size) return nullptr;

    return identifiers[ndx];
}