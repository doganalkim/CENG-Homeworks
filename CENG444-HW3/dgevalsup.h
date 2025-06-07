#ifndef __DGEVALSUP
#define __DGEVALSUP

// Macro definitions for the operator and instruction codes.
// Some of the instruction codes are for future use.
#define OP_NOP    0
#define OP_COMMA	1
#define OP_ASSIGN	2
#define OP_COND	3
#define OP_ALT    4
#define OP_BAND	5
#define OP_BOR		6
#define OP_EQ		7
#define OP_NEQ		8
#define OP_LT		9
#define OP_LTE		10
#define OP_GT		11
#define OP_GTE		12
#define OP_ADD		13
#define OP_SUB		14
#define OP_MUL		15
#define OP_DIV		16
#define OP_MINUS  17
#define OP_NOT		18
#define OP_AA     19
#define OP_CALL   20
#define LAST_OP   (OP_CALL)
#define JMP			(LAST_OP+1)
#define JF			(LAST_OP+2)
#define INSID     (LAST_OP+3)
#define CONST     (LAST_OP+4)

// A few forward declarations. Thanks C++! You have to be single pass.
class MyParser;
class DGEvalNode;
class DGEvalStatementNode;
class DGEvalExpNode;
class DGEvalIdentifierList;
class DGEvalStatementList;
class DGEvalFuncDesc;
class DGEvalMsgDesc;
class DGEvalMsgSet;
class DGEvalSymbolTable;
class DGEvalSymbolDesc;

// User message severity levels.
enum class DGEvalMsgSeverity
{
   Error=0,
   Warning=1,
   Info=2
};

// Type encoding
enum class DGEvalType
{
   DGNumber=1,
   DGString=2,
   DGBoolean=3,

   // Note that the array code is temporary and replaced by
   // the type coupled with dimension.
   DGArray=4
};

// Type descriptor. This is good enough to describe DGEval types.
struct DGTypeDesc
{
   // Basic type. DGArray must not exist here when finalized.
   DGEvalType  type;
   // Space dimension. Zero for no array. Any positive number is space
   // dimenstion count.
   int         dim;
   bool operator == (DGTypeDesc other);
   bool operator != (DGTypeDesc other);
};

class DGEvalMsgSet
{
      // The vector of the message descriptor.
      vector<DGEvalMsgDesc *> messages;
   public:
      DGEvalMsgSet();
      virtual ~DGEvalMsgSet();

      static bool compareMessageLines(DGEvalMsgDesc *p1, DGEvalMsgDesc *p2);
      void appendMessage(int pLineNumber, string pMsg, DGEvalMsgSeverity pS);
      void sort();

      // Generates a JSON value, which is array of the messages contained.
      void writeAsJSON(ostream *os);
};

class DGEvalSymbolTable
{
      // The vector of the symbols in the table.
      // The symbol table was kept simple for ease of implementation.
      vector<DGEvalSymbolDesc *>   symbols;
   public:
      DGEvalSymbolTable();
      virtual ~DGEvalSymbolTable();

      // Student assignment part

      // Finds the symbol specified by "name". Returns descriptor when found, nullptr otherwise.
      DGEvalSymbolDesc *findSymbol(string name);
      // Inserts the symbol specified by "name". .
      DGEvalSymbolDesc *addSymbol(string name, DGTypeDesc pType);

      // Generates a JSON value, which is array of the symbols contained.
      void writeAsJSON(ostream *os);
};


class DGEval
{
   public:
      static string           // An array of operator describing strings that can be accessed by the op-codes.
                              opStr[],
                              // An array of type describing string for reporting. This can be accessed by the type enumeration "DGEvalType".
                              typeStr[],
                              // An array of severity describing string for user message reporting.
                              // This can be accessed by the type enumeration "DGEvalMsgSeverity".
                              severityStr[];
      static DGEvalFuncDesc   // The library structure, which is static for ease of implementation.
                              // Ideally, this is replaced by a model that supports dynamic library management.
                              lib[];
      static int              // This is the sorting vector for the "lib". The symbol resolution for the runtime library
                              // uses binary search that requires sorted entities.
                             *sVector;

      // Sorter comparison callback for the runtime library.
      // The library is sorted once.
      static int compareFDesc(const void *v1, const void *v2);

      DGEvalStatementList     // The list of the recognized statements. This will be reported by the project.
                             *statements=nullptr,
                              // The topologically ordered list of the statements having
                              // no circular reference conflict. These are the survivors
                              // that will be processed further. This will be reported by the project.
                             *draftedStatements;

      DGEvalMsgSet            // The set of the messages that will be part of the project output.
                              messageSet;
      DGEvalSymbolTable       // The table of the symbols defined by the DGEval program. This will be reported by the project.
                              symbolTable;
      MyParser                // The driver (parser) reference. It may come handy!
                             *parser;

      DGEval(MyParser *pParser);
      virtual ~DGEval();

      // Finds the descriptor of the function referred by the "fName". Returns nullptr if the function is not found.
      static DGEvalFuncDesc *findLibFunction(string fName);

      // Student assignment part

      // Scans the statements to generate symbolic dependencies.
      void scanDependencies();
      // Scans the the expression nodes to generate symbolic dependencies.
      void scanDependencies(DGEvalStatementNode *statement, DGEvalExpNode *parentNode, DGEvalExpNode *node);
      // Scans the drafted statements to calculate types of the expressions.
      void scanCalculateTypes();
      // Scans the expression nodes to calculate types of the expressions.
      void scanCalculateTypes(DGEvalExpNode *parentNode, DGEvalExpNode *node);
      // Generates a vector of the expression parts separated by comma operators.
      vector<DGEvalExpNode *> expressionPartVector(DGEvalExpNode *node);
      // Builds the statement dependency lists. Each statement has a list of statements that
      // must be executed before.
      void buildStatementDependencyLists();
      // Performs topological sort by pruning and drafting independent statements recursively.
      // Efectively, this is a transfer from the list of "statements" to the list of "draftedStatements".
      void draftIndependents();

      // Generates a JSON value, which contains the whole result of language processing. See the samples.
      virtual void writeAsJSON(ostream *os);
};

class DGEvalNode
{
   public:
      int   // The number of the relevant line for this node.
            lineNumber=0;

      DGEvalNode(int pLineNumber);
      virtual ~DGEvalNode();

      // Produces a string constant to ease JSON generation.
      void writeStrValue(ostream *os, string *strValue);

      // Generates a JSON value that conveys details of the node.
      // This must be overridden by concrete subclasses.
      virtual void writeAsJSON(ostream *os)=0;
};

// Represents an entry in a statement dependency list.
// Each statement maintains a statement dependency list that defines
// the dependency graph that needs to be sorted topologically.
// See statementDependencyList member of DGEvalStatementNode.
struct DGEvalDependencyEntry
{
   DGEvalStatementNode     // The statement being dependent on.
                          *statement;
   DGEvalDependencyEntry   // Pointer to the next statement
                          *next;
};

class DGEvalStatementNode : public DGEvalNode
{
   public:
      DGEvalExpNode           // The root node of the expression tree owned by this statement.
                             *exp;
      DGEvalIdentifierList    // The list of the identifiers defined by this statement.
                              // Note that any assignment operator implies a definition.
                             *defining,
                              // The list of the identifiers being referred to by the expression
                              // owned by this statement. Runtime library function names
                              // are not placed in this list.
                             *symbolDependency;
      DGEvalDependencyEntry   // Represents an entry in a statement dependency list, which is singly linked list.
                              // Each statement maintains a statement dependency list that defines
                              // the dependency graph that needs to be sorted topologically.
                             *statementDependencyList=nullptr;
      DGEvalStatementNode     // The next statement in the list of statements.
                              // See DGEvalStatementList.
                             *next,
                              // The previous statement in the list of statements.
                              // See DGEvalStatementList.
                             *prev;

      DGEvalStatementNode(int pLineNumber, DGEvalExpNode *pExp);
      virtual ~DGEvalStatementNode();

      // Student assignment part
      // Creates and places a symbol that is defined by this statement. "defining" receives the symbol entry.
      // Duplicates are prevented
      void addDefining(string *symbol);
      // Creates and places a symbol that must have been calculated before execution of this statement..
      // "symbolDependency" receives the symbol entry. Duplicates are prevented
      void addDependency(string *symbol);
      // The method that creates additional symbols out of expressions that will generate dependency.
      // Creates and places the symbols that must have been calculated before execution of this statement..
      // "symbolDependency" receives the symbol entry. Duplicates are prevented.
      // At this level, it does nothing.
      virtual void addExplicitDependencies();

      // Returns true if the specified "identifier" is defined by this statement.
      bool doesDefine(string *identifier);
      // Places the specified "statement" to the list of "statementDependencyList".
      void addPredecessor(DGEvalStatementNode *statement);
      // Removes the specified "statement" from the list of "statementDependencyList".
      bool removePredecessor(DGEvalStatementNode *statement);

};

class DGEvalExpStatement : public DGEvalStatementNode
{
   public:
      DGEvalExpStatement(int pLineNumber, DGEvalExpNode *pExp);
      virtual ~DGEvalExpStatement();

      // Student assignment part
      // Generates a JSON value that covers the expression tree.
      virtual void writeAsJSON(ostream *os);
};

class DGEvalWaitStatement : public DGEvalStatementNode
{
   public:
      DGEvalIdentifierList    // The list of the identifiers specified in the wait statement code.
                             *identifierList;

      DGEvalWaitStatement(int pLineNumber, DGEvalExpNode *pExp, DGEvalIdentifierList *pIdentifierList);
      virtual ~DGEvalWaitStatement();

      // Student assignment part
      // The method that creates additional symbols out of expressions that will generate dependency.
      // Creates and places the symbols that must have been calculated before execution of this statement..
      // "symbolDependency" receives the symbol entry. Duplicates are prevented.
      // At this level it injects the items from the "identifierList".
      virtual void addExplicitDependencies();
      // Generates a JSON value that covers identifiers and the expression tree.
      virtual void writeAsJSON(ostream *os);
};

class DGEvalExpNode : public DGEvalNode
{
   public:
      DGEvalExpNode  // The child node on the left. This is null for a leaf.
                    *left,
                     // The child node on the right. This is null for a leaf.
                    *right;
      int            // The op-code of the node. This is one of the op-code macros.
                     opCode;
      DGTypeDesc     // The type of the code initialized as zero {type:none, dim:0};
                     type{};

      // Union of the constants depending on the constant type.
      // stringValue is also used for the ids.
      union
      {
         double      doubleValue;
         string     *stringValue;
         bool        boolValue;
      };

      DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode);
      DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode, DGEvalExpNode *pRight);
      DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pOpCode, DGEvalExpNode *pRight, DGEvalExpNode *pThird);
      DGEvalExpNode(int pLineNumber, double value, int pOpCode);
      DGEvalExpNode(int pLineNumber, string *value, int pOpCode);
      DGEvalExpNode(int pLineNumber, bool value, int pOpCode);
      DGEvalExpNode(int pLineNumber, DGEvalExpNode *arrayRoot);
      virtual ~DGEvalExpNode();

      // Student assignment part
      virtual void writeAsJSON(ostream *os);
};

// This will be required for the next experiment. No details will be given for the moment.
class DGEvalArray
{
   public:
      DGEvalType   type;
      union
      {
         vector<double>          doubleArr;
         vector<string *>        stringArr;
         vector<bool>            boolArr;
         vector<DGEvalArray *>   arrArr;
      };

      DGEvalArray(int cnt, double *values);
      DGEvalArray(int cnt, string **values);
      DGEvalArray(int cnt, bool *values);
      DGEvalArray(int cnt, DGEvalArray *values);
};

class DGEvalIdentifierList
{
      // Implemented as a wrapper around a vector.
      vector<string *>  identifiers;
   public:
      DGEvalIdentifierList();
      DGEvalIdentifierList(string *identifier);
      virtual ~DGEvalIdentifierList();

      // Student assignment part
      // Appends the given "identifier" to the "identifiers" allowing duplicates
      void append(string *identifier);
      // Appends the given "symbol" to the "identifiers" preventing duplicates.
      bool appendUnique(string *symbol);
      // Appends the items found in the given list to the "identifiers" preventing duplicates.
      bool appendUnique(DGEvalIdentifierList *otherList);
      // Returns the number of items in the list.
      int size();
      // Returns the identifier found with the zero based index "ndx". When not found, returns nullptr.
      string *identifierAt(int ndx);
      // Generates a JSON value that conveys the content as an array of strings.
      virtual void writeAsJSON(ostream *os);
};

class DGEvalStatementList
{
   public:
      // Doubly linked list implementation.
      DGEvalStatementNode    *head=nullptr,
                             *tail=nullptr;

      DGEvalStatementList();
      DGEvalStatementList(DGEvalStatementNode *statement);
      virtual ~DGEvalStatementList();

      // Student assignment part
      // Appends a statement to the list.
      void append(DGEvalStatementNode *statement);
      // Removes a statement from the list.
      void remove(DGEvalStatementNode *statement);
      // Generates a JSON array that contains the statements.
      // For the drafted statements, the list must be in the order of execution.
      virtual void writeAsJSON(ostream *os);
};

// A descriptor class for a runtime library function.
// Currently, it lacks a pointer to the implementing code.
class DGEvalFuncDesc
{
   public:
      string      // The name of the function.
                  name;
      int         // Number of the parameters for this function.
                  paraCount;
      DGTypeDesc  // Type descriptor array to describe the return type,
                  // and the parameter types. İtem 0 is the return value.
                  // This is kept simple for ease of implementation.
                  // Ideally, this is variable based on the number of the parameters.
                  types[3];
};

// A simple class to represent a symbol with a name and type.
class DGEvalSymbolDesc
{
   public:
      string      // The name of the symbol.
                  name;
      DGTypeDesc  // The type specifier for the symbol.
                  type;

      DGEvalSymbolDesc(string pName, DGTypeDesc pType)
      {
         name=pName;
         type=pType;
      }
};

// A simple descriptor for a user message.
class DGEvalMsgDesc
{
   public:
      int               // THe number of the relevant line. INT_MAX and INT_MIN specify
                        // that the line number will not be reported.
                        lineNumber;
      string            // The message text
                        msg;
      DGEvalMsgSeverity // Severity level of the message.
                        s;

      DGEvalMsgDesc(int pLineNumber, string pMsg, DGEvalMsgSeverity pS);
};

#endif
