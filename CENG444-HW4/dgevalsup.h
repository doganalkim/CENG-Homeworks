#ifndef __DGEVALSUP
#define __DGEVALSUP

#include <cstdint>

// Macro definitions for the operator and instruction codes.
// Some of the instruction codes are for future use.
#define OP_NOP    0
#define OP_COMMA	1
#define OP_ASSIGN	2
#define OP_COND   3
#define OP_ALT    4
#define OP_BAND   5
#define OP_BOR	   6
#define OP_EQ	   7
#define OP_NEQ	   8
#define OP_LT     9
#define OP_LTE	   10
#define OP_GT     11
#define OP_GTE	   12
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
#define JT        (LAST_OP+3)
#define INSID     (LAST_OP+4)
#define CONST     (LAST_OP+5)
#define LRT       (LAST_OP+6)
#define POP       (LAST_OP+7)

#define OPTIMIZE_DC_STATEMENT 1  // Dead code elimination on ineffective statements
#define OPTIMIZE_DC_EXPPART   2  // Dead code elimination with expression parts having no effect
#define OPTIMIZE_PH_OFFLOAD   4  // Peephole optimization to eliminate re-loading right after popping
#define OPTIMIZE_PH_CONSTSINK 8  // Peephole optimization to eliminate push-pop sequence on a constant
#define OPTIMIZE_ALL (OPTIMIZE_DC_STATEMENT | OPTIMIZE_DC_EXPPART | OPTIMIZE_PH_OFFLOAD | OPTIMIZE_PH_CONSTSINK)

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
class DGEvalIC;
class DGEvalICInst;
class DGEvalCodeLocation;

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
   DGNone=0,

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

class DGEvalICInst
{
   public:
      int            // The opCode of the instruction. By default, the op code is OP-NOP
                     opCode=OP_NOP;
      int            // The immediate parameter value stored. Number of parameters, number of pops
                     // and similar data are stored in this member.
                     p1=0,
                     // The x64 code location where the first CPU instruction resides in
                     // target.
                     codeOffset=0;
      DGTypeDesc     // The type data for the instruction
                     type{};

      // Additional data associated with the instruction. Immediate values,
      // function indexes are stored here
      union
      {
         string        *strConstant=nullptr;
         double         numConstant;
         bool           boolConstant;
      };

      DGEvalICInst(); // Default constructor requirement inherent from code location vector. Not used at all.
      DGEvalICInst(int pOpCode, int pP1);
      DGEvalICInst(int pOpCode, int pP1, DGTypeDesc pType);
      virtual ~DGEvalICInst();

      virtual void writeAsJSON(ostream *outStream);
};

// Type alias for ease of coding. A code path is a vector of
// code locations.
typedef vector<int> DGEvalCodePath;

class DGEvalCodePathWindow
{
      void build(DGEvalIC *ic, int codeLocationIndex, int windowSize, int depth, int *builder);
   public:
      // The vector of the code paths.
      vector<DGEvalCodePath *>   path;

      DGEvalCodePathWindow();
      virtual ~DGEvalCodePathWindow();
      void build(DGEvalIC *ic, int codeLocationIndex, int windowSize);
};

class DGEvalCodeLocation
{
      // Calculates the predecessors.
      // The accumulator vector receives index of predecessor code location
      // for the possible flows.
      void           predecessors(vector<int> *accumulator);

      // Checks whether the specified code location index exists in the specified vector.
      bool           ivHasNdx(vector<int> *v, int ndx);

      // Appends the specified code location index to the specified vector preventing duplicates.
      void           ivAppend(vector<int> *v, int ndx);

      // Appends the specified code location vector to the specified vector preventing duplicates.
      void           ivAppend(vector<int> *v, vector<int> *s);
   public:
      DGEvalICInst   // The resident instruction.
                     inst;
      DGEvalIC       // Pointer to the owning IC
                    *ic=nullptr;

      // Relocation offset value for code motion when applied.
      // IC level peephole optimization moves the linear code to shrink its size.
      int            relocation=0;
      vector<int>    // The vector of incoming jumps
                    *ij=nullptr;

      DGEvalCodeLocation(); // This is the automatic constructor only for IC resizing required. Not used at all!
      DGEvalCodeLocation(DGEvalIC *pIc, DGEvalICInst i);
      ~DGEvalCodeLocation();

      // Registers a code location as an immediate flow-predecessor
      void registerPredecessor(int source);

      // Calculates the predecessors by avoiding code location revisits.
      // The accumulator vector receives index of predecessor code location
      // for the possible flows.
      vector<int> *predecessors();
};

// The class that represents the linear IR (Intermediate code)
class DGEvalIC
{
   public:
      // The main storage of the instructions.
      vector<DGEvalCodeLocation> code=vector<DGEvalCodeLocation>();

      DGEvalIC();
      virtual ~DGEvalIC();

      // Emits an instruction with the specified opCode and parameter
      DGEvalICInst *emitIC(int opCode, int p1);
      // Emits an instruction with the specified opCode, parameter, and type
      DGEvalICInst *emitIC(int opCode, int p1, DGTypeDesc type);

      // Retrieves the instruction at the specified code location.
      DGEvalICInst *instructionAt(int ndx);

      // Retrieves the code locationat the specified code location.
      DGEvalCodeLocation *codeLocationAt(int ndx);

      // Marks the instruction at the specified code location for removal
      bool markRemoval(int ndx, int count);

      // Removes all the instructions marked for removal by adjusting
      // jump targets and predecessors
      int applyRemoval();

      int instCount();
      int codeLocationIndex(DGEvalCodeLocation *cl);

      // Generates immediate jump vectors for each code location.
      void collectImmediateJumps();

      virtual void writeAsJSON(ostream *outStream);
      void writeReadable(ostream *outStream);
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
      int messageCount(DGEvalMsgSeverity type);

      // Generates a JSON value, which is array of the messages contained.
      void writeAsJSON(ostream *os);

      // Reports to the standard output
      void writeToConsole();
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
      // Retrieves the symbol with the given index
      DGEvalSymbolDesc *symbolAt(int ndx);
      // Returns the number of the symbols stored.
      int symbolCount();

      // Generates a JSON value, which is array of the symbols contained.
      void writeAsJSON(ostream *os);
};

class DGEvalArray
{
   public:
      DGTypeDesc type;

      DGEvalArray(DGTypeDesc pType)
      {
         type=pType;
      }

      virtual ~DGEvalArray() {};

      virtual bool equalsToOther(DGEvalArray *other)=0;
};

template <typename tp> class DGEvalArrayType : public DGEvalArray
{
   public:
      vector<tp> *arr;

      DGEvalArrayType(DGTypeDesc pType) : DGEvalArray(pType)
      {
         arr=new vector<tp>;
      }

      ~DGEvalArrayType()
      {
         delete arr;
      }

      // Virtual method that compares to given elements.
      // Must be overridden for each needed type.
      virtual bool areEqualElements(tp p1, tp p2)=0;

      // Compares the array with another array. Returns true if this array's
      // content equals the others in terms of basic values.
      virtual bool equalsToOther(DGEvalArray *other)
      {
         return equalsTo((DGEvalArrayType<tp> *)other);
      }

      // Generic comparison method for the arrays.
      // equalsToOther must be used
      bool equalsTo(DGEvalArrayType<tp> *other)
      {
         int  cnt=arr->size();
         vector<tp> *otherArr=other->arr;
         bool test=cnt==(int)otherArr->size();

         for (int i=0;test && i<cnt;i++)
            test=areEqualElements((*arr)[i], (*otherArr)[i]);
         return test;
      }
};

// DGEvalArray descendant for string
class DGEvalArrayString : public DGEvalArrayType<string *>
{
   public:
      DGEvalArrayString();
      DGEvalArrayString(string **base, int count);

      virtual ~DGEvalArrayString();

      virtual bool areEqualElements(string *p1, string *p2);
};

// DGEvalArray descendant for number
class DGEvalArrayDouble : public DGEvalArrayType<double>
{
   public:
      DGEvalArrayDouble();
      DGEvalArrayDouble(double *base, int count);
      virtual ~DGEvalArrayDouble();

      virtual bool areEqualElements(double p1, double p2);

      // Followings are for DGEval runtime library implementation. Names are self explanatory.
      double stddev();
      double mean();
      double count();
      double min();
      double max();
};

// DGEvalArray descendant for boolean
class DGEvalArrayBool : public DGEvalArrayType<bool>
{
   public:
      DGEvalArrayBool();
      DGEvalArrayBool(int64_t *base, int count);
      virtual ~DGEvalArrayBool();

      virtual bool areEqualElements(bool p1, bool p2);
};

// DGEvalArray descendant for array
class DGEvalArrayArray : public DGEvalArrayType<DGEvalArray *>
{
   public:
      DGEvalArrayArray(DGTypeDesc td);
      DGEvalArrayArray(DGTypeDesc td, DGEvalArray **base, int count);
      virtual ~DGEvalArrayArray();

      virtual bool areEqualElements(DGEvalArray *p1, DGEvalArray *p2);
};

// Type alias for void function with no parameter.
// The x64 code generated by DGEval is of this type.
typedef void DGEvalDynamicFunc();

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
                             *statements,
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

      int                     // The requested optimizations. See the OPTIMIZATION_... constants.
                              optimization;
      DGEvalIC                // Linear IR object that will be generated when no syntactic and
                              // semantic problems are found.
                             *ic=nullptr;
      DGEvalDynamicFunc       // Generates x64 executable function using the IC
                             *generateCode();

      vector<string *>        // Accumulates the strings created during evaluations.
                              // Cleanup procedure (postexecutecleanup) frees the string objects.
                              stringVectorToClean;
      vector<DGEvalArray *>   // Accumulates the arrays created during evaluations.
                              // Cleanup procedure (postexecutecleanup) frees the string objects.
                              arrayVectorToClean;
      bool                    // True means the execution was interrupted with runtime error.
                              // This works only for array boundary check currently.
                              // Check code is veawed by the code generator after each array access.
                              exception=false;

      DGEval(MyParser *pParser);
      virtual ~DGEval();

      // Finds the descriptor of the function referred by the "fName". Returns nullptr if the function is not found.
      static DGEvalFuncDesc *findLibFunction(string fName);
      // Returns the descriptor of the function having the specified index.
      static DGEvalFuncDesc *libFunctionAt(int ndx);

      // Student assignment part

      // Scans the statements to generate symbolic dependencies.
      void scanDependencies();
      // Scans the the expression nodes to generate symbolic dependencies.
      void scanDependencies(DGEvalStatementNode *statement, DGEvalExpNode *parentNode, DGEvalExpNode *node);
      // Scans the drafted statements to calculate types of the expressions.
      void scanCalculateTypes();
      // Scans the expression nodes to calculate types of the expressions.
      void scanCalculateTypes(DGEvalExpNode *parentNode, DGEvalExpNode *node);

      // Scans the drafted statements to apply constant folding.
      void scanConstantFolding();
      // Scans the expression nodes to perform constant folding in expressions.
      void scanConstantFolding(DGEvalExpNode *parentNode, DGEvalExpNode *node);

      // Transforms a string constant opCode to the relevant LRT 3
      void transformStrConstToLRT(DGEvalExpNode *node);

      // Scans the drafted statements to generate intermediate code.
      void scanForIC();
      // Scans the expression nodes to generate intermediate code.
      void scanForIC(DGEvalExpNode *parentNode, DGEvalExpNode *node);

      // Scans the linear IR
      void peepholeIC();

      // Registers string objects for cleanup after execution
      void registerStringObject(string *str);
      // Registers string objects for cleanup after execution
      void registerDGArrayObject(DGEvalArray *arr);

      // Destroys the given tree by applying proper traversal.
      DGEvalExpNode *destroyExpTree(DGEvalExpNode *node);

      DGEvalStatementNode *statementInScan=nullptr;
      // Generates a vector of the expression parts separated by comma operators.
      vector<DGEvalExpNode *> *expressionPartVector(DGEvalExpNode *node);
      // Builds the statement dependency lists. Each statement has a list of statements that
      // must be executed before.
      void buildStatementDependencyLists();
      // Performs topological sort by pruning and drafting independent statements recursively.
      // Effectively, this is a transfer from the list of "statements" to the list of "draftedStatements".
      void draftIndependents();

      // Produces a string constant to ease JSON generation.
      static void writeStrValue(ostream *os, string *strValue);


      // Generates a JSON value, which contains the whole result of language processing. See the samples.
      virtual void writeAsJSON(ostream *os);

      // LRT Start

      // 0: Allocates an array with the elements on the stack and the supplied type descriptor.
      static DGEvalArray *allocatearray(DGEval *dgEval, DGTypeDesc signature, int itemCount, uint64_t *base);

      // 1: Returns an element in an array. Returns the array object passed (arr).
      static uint64_t arrayelement(DGEval *dgEval, DGEvalArray *arr, int64_t index);

      // 2: Appends an element to an array. Returns the array object passed (arr).
      static DGEvalArray *appendelement(DGEvalArray *arr, uint64_t para);

      // 3: Allocates a string given a string constant detected in source.
      static string *allocatestring(DGEval *dgEval, string *s1);

      // 4: Concatenates two strings and returns the concatenated string.
      static string *catstring(DGEval *dgEval, string *s1, string *s2);

      // 5: Converts a number to string.
      static string *number2str(DGEval *dgEval, double n);

      // 6: Compares two strings an returns the test result as true or false.
      // test parameter encoding: 0->EQ, 1:NEQ, 2->GT, 3->LT, 4->GTE, 5->LTE
      static int64_t strcmp(string *s1, string *s2, int64_t test);

      // 7: Compares two arrays. Returns true in case of equality, false otherwise.
      static int64_t arrcmp(DGEvalArray *arr1, DGEvalArray *arr2);

      // 8: Performs clean-up after execution. This must be called before the final return instruction.
      static int64_t postexecutecleanup(DGEval *dgEval);

      // 9: Checks for an exception and returns true if an exception record was generated.
      static int64_t checkexception(DGEval *dgEval);

      // LRT End

      // Library Start

      static double stddev(DGEvalArrayDouble *array);
      static double mean(DGEvalArrayDouble *array);
      static double count(DGEvalArrayDouble *array);
      static double min(DGEvalArrayDouble *array);
      static double max(DGEvalArrayDouble *array);
      static double sin(double number);
      static double cos(double number);
      static double tan(double number);
      static double pi();
      static double atan(double number);
      static double asin(double number);
      static double acos(double number);
      static double exp(double number);
      static double ln(double number);
      static double print(string *str);
      static double random(double number);
      static double len(string *str);
      static string *right(DGEval *dgEval, string *str, double n);
      static string *left(DGEval *dgEval, string *str, double n);

      // Library End
};

class DGEvalNode
{
   public:
      int   // The number of the relevant line for this node.
            lineNumber=0;

      DGEvalNode(int pLineNumber);
      virtual ~DGEvalNode();

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
      int                     // When this statement appears as a predecessor of another statement,
                              // this count gets incremented.
                              // This helps identify statements that is going to be eliminated.
                              asPredecessorCount=0;

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

      vector<DGEvalExpNode *>
                     // The parameter vector for the call operator
                    *pVector=nullptr;
      int            // The code location index where OP_ALT code for true ends
                     altTrueEnd=-1,
                     // The code location index where OP_ALT code for false ends
                     altFalseEnd=-2,
                     // This member holds LRT or RTL index values.
                     idNdx=-1,
                     // This member useful while checking accumulated comma operands.
                     stackLoad=1,
                     // The number of the function calls found in this expression.
                     // This helps identify the code fragments that are going to be eliminated.
                     functionCallCount=0,
                     // The number of the assignments found in this expression.
                     // This helps identify the code fragments that are going to be eliminated.
                     assignmentCount=0,
                     // When true, IC instruction emitter will skip this node.
                     // This is for marking the nodes that will lead to dead code generation.
                     // This occurs when a sub-expression is coded with comma operators not accessing
                     // a function or not forming an array literal.
                     eliminateIC=false;

      // Union of the constants depending on the constant type.
      // stringValue is also used for the ids.
      // doubleValue is also used as call index value (call, crt)
      union
      {
         double      doubleValue;
         string     *stringValue;
         bool        boolValue;
      };

      // Constructor in various tastes.
      DGEvalExpNode(int pLineNumber, DGEvalExpNode *pLeft, int pPpCode);
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
      //DGEvalStatementList(DGEvalStatementNode *statement);
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
      void        // The entry point of the function
                 *f;
      bool        // True if the function requires implicit pointer to the DGEval instance.
                  // This is necessary for resource allocating functions to enable
                  // post execution clean-up.
                  // See DGEval::registerStringObject(string *str)
                  // and DGEval::registerDGArrayObject(DGEvalArray *arr)
                  requiresDGEval;
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
      int         // Symbol index
                  ndx=-1;

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
