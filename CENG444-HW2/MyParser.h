/*
 * MyParser.h
 *
 *  Created on: Mar 24, 2024
 *      Author: erkan
 */

 #ifndef MYPARSER_H_
 #define MYPARSER_H_

#include <fstream>

 namespace yy
 {
    class MyParserBase;
 };
 
 // Following enumeration may be useful when downcasting is needed.
 enum class JValueType
 {
    Str = 0,
    Num = 1,
    Dict = 32,
    Arr = 33
 };
 
 class MyFlexLexer;
 
 class MyParser
 {
   yy::MyParserBase *base;
   MyFlexLexer      *lexer;

   std::ofstream outputFile;

   int current_line;
   int total_assignments;
   int total_evaluations;
   int total_conditionals;
   int total_arrays;
   int total_nums;
   int total_strs;
   int total_arrayAccess;
   int total_functionCall;

   bool              parseError;
 public:
   MyParser();
   ~MyParser();

   void parse(yy::MyParserBase *base, ifstream *is);
   int lex(yy::MyParserBase::value_type *lval);
   void finalize();

   void setValue(JValueType t);
   void storeMember();
   void storeArrElement();
   void nameFromId();
   void nameFromStr();

   int get_id();
   int get_str();
   int get_number();
   int get_bool();
   int get_wait();
   int get_then();

   void setParseError();
   bool getParseError();

   void new_line_detected();
   void print_cur_line();

   void assignment_detected();
   void evaluation_detected();
   void conditional_detected();
   void array_detected();
   void num_detected();
   void str_detected();
   void arrayAccess_detected();
   void functionCall_detected();
};
 
 
 #endif /* MYPARSER_H_ */
 