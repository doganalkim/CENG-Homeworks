using namespace std;

#include <iostream>
#include <fstream>
#include <string.h>
#include "project02.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"

MyParser::MyParser(){
   parseError=false;
   base=nullptr;
   lexer=nullptr;
   current_line = 1;
   total_assignments = 0;
   total_evaluations = 0;
   total_conditionals = 0;
   total_arrays = 0;
   total_nums = 0;
   total_strs = 0;
}

MyParser::~MyParser(){
   // Clean-up
   delete lexer;
   delete base;
}

int MyParser::lex(yy::MyParserBase::value_type *lval){
   return lexer->lex(lval);
}

void MyParser::parse(yy::MyParserBase *base, ifstream *is){
   this->base=base;
   lexer=new MyFlexLexer();
   lexer->switch_streams(is);
   base->parse();
   outputFile.open("project02syn.txt");
}

void MyParser::finalize(){
   outputFile << total_assignments << endl;
   outputFile << total_evaluations << endl;
   outputFile << total_conditionals << endl;
   outputFile << total_arrays << endl;
   outputFile << total_nums << endl;
   outputFile << total_strs << endl;
   outputFile << total_arrayAccess << endl;
   outputFile << total_functionCall << endl;
}

int MyParser::get_id(){
   return yy::MyParserBase::token::ID;
}

int MyParser::get_str(){
   return yy::MyParserBase::token::STR;
}

int MyParser::get_number(){
   return yy::MyParserBase::token::NUM;
}

int MyParser::get_bool(){
   return yy::MyParserBase::token::BOOL;
}

int MyParser::get_wait(){
   return yy::MyParserBase::token::WAIT;
}

int MyParser::get_then(){
   return yy::MyParserBase::token::THEN;
}

void MyParser::new_line_detected(){
   // cout << "Line:" << current_line << endl;
   // cout << "FC:" << total_functionCall << endl;
   current_line++;
}

void MyParser::print_cur_line(){
   cout << "Line:" << current_line  <<  ": Syntax error."  << endl;
}

void MyParser::nameFromId(){
}

void MyParser::nameFromStr(){
}

void MyParser::setValue(JValueType t){
   // Mock action using recognized value type
   switch (t)
   {
      case JValueType::Str:
         break;
      case JValueType::Num:
         break;
      case JValueType::Dict:
         break;
      case JValueType::Arr:
         break;
   }
}

void MyParser::storeMember(){
   // Mock action to store recognized member
}

void MyParser::storeArrElement(){
   // Mock action to store array element
}

void MyParser::setParseError(){
   parseError=true;
}

bool MyParser::getParseError(){
   return parseError;
}

void MyParser::assignment_detected(){
   total_assignments++;
}

void MyParser::evaluation_detected(){
   total_evaluations++;
}

void MyParser::conditional_detected(){
   total_conditionals++;
}

void MyParser::array_detected(){
   total_arrays++;
}

void MyParser::num_detected(){
   total_nums++;
}

void MyParser::str_detected(){
   total_strs++;
}

void MyParser::arrayAccess_detected(){
   total_arrayAccess++;
}

void MyParser::functionCall_detected(){
   total_functionCall++;
}

extern MyParser *parser;

void yy::MyParserBase::error(const std::string &msg){
   parser->setParseError();
   cout << "Not recognized!" << endl;
   parser -> print_cur_line();
}
